/*
	This file is part of Overmix.

	Overmix is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Overmix is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Overmix.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CommandParser.hpp"
#include "Parsing.hpp"
#include "Parsers.hpp"
#include "Processor.hpp"

#include "containers/ImageContainer.hpp"
#include "containers/ImageContainerSaver.hpp"
#include "video/VideoStream.hpp"
#include "video/VideoFrame.hpp"
#include "planes/SubtitleDetector.hpp"
#include "planes/SceneDetector.hpp"

#include <QFileInfo>
#include <QImage>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

#include <iostream>
#include <vector>

using namespace Overmix;

static void printHelp( QString type ){
	QTextStream std( stdout );
	
	std << "OvermixCli [files]... [--command=arguments]...\n";
	std << "Version " << OVERMIX_VERSION_STRING << "\n\n";
	
	
	if( type.isEmpty() ){
		std << "Available commands:\n";
		std << "\t" << "--pre-process\n";
		std << "\t" << "--comparator\n";
		std << "\t" << "--align\n";
		std << "\t" << "--render\n";
		std << "\t" << "--post-process\n";
		std << "\t" << "--save\n";
		std << "\t" << "--remove-subtitles\n";
		std << "\t" << "--scene-detect\n";
		std << "\t" << "--no-gui\n";
		std << "\t" << "--help\n";
		std << "\n";
		std << "Type '--help=render' to read more about the command '--render'.\n";
		std << "Each command will be applied in order, so load the images before aligning, etc.\n";
		std << "Usually you would have something like this:\n";
		std << "OvermixCli *.png --align=... --render=... --save=0:output.png";
	}
	else{
		if( type == "pre-process" ){
			std << "Applies a processing step on all input images:\n\n";
			processingHelpText( std );
		}
		else if( type == "post-process" ){
			std << "Applies processing on the rendered image with id 'n':\n";
			std << "--post-process=n:<processing options>\n\n";
			processingHelpText( std );
		}
		else if( type == "align" )
			alignerHelpText( std );
		else if( type == "comparator" )
			comparatorHelpText( std );
		else if( type == "render" )
			renderHelpText( std );
		else if( type == "save" ){
			std << "Saves the 'n' rendered image, for example:\n";
			std << "--save=0:output.png\n";
		}
		else if( type == "no-gui" )
			std << "Prevents the GUI from opening, only applies for 'Overmix' executable\n";
		else if( type == "remove-subtitles" ){
			std << "Detects and removes subtitle regions from images:\n";
			std << "--remove-subtitles\n\n";
			std << "Uses adaptive thresholding and morphological operations to detect subtitles.\n";
			std << "The subtitle region (typically bottom 15% of the image) will be cropped.\n";
			std << "This is useful for stitching anime pan shots with subtitles.\n";
		}
		else if( type == "scene-detect" ){
			std << "Detects scene transitions in image sequence:\n";
			std << "--scene-detect[=<threshold>]\n\n";
			std << "Threshold: 0.0-1.0 (default: 0.3)\n";
			std << "Uses SAD (Sum of Absolute Differences) between adjacent frames.\n";
			std << "Outputs scene information and keeps only the longest scene.\n";
			std << "This is useful for removing unwanted frames before/after the target pan shot.\n";
		}
		else
			std << "Unknown command: " << type << "\n";
	}
}


struct Command{
	bool is_file;
	Splitter parts;
	
	explicit Command( QString cmd )
		:	is_file( !cmd.startsWith( "--" ) )
		,	parts( cmd.right( is_file ? cmd.length() : cmd.length()-2 ), '=' )
		{ }
	
	template<typename T>
	bool is( T name ) const { return parts.left == name; }
	QString filename() const { return parts.left; }
	QString arguments() const { return parts.right; }
};


void CommandParser::parse( QStringList commands ){
	std::vector<ImageEx> renders;
	
	for( auto& cmd_str : commands ){
		Command cmd(cmd_str);
		if( cmd.is_file ){ //Load a file
			if( QFileInfo( cmd.filename() ).completeSuffix() == "xml.overmix" )
				ImageContainerSaver::load( images, cmd.filename() );
			else if( VideoStream::isVideoFile( cmd.filename() ) )
			{
				try{
					VideoStream stream( cmd.filename() );
					while (true){
						images.addImage( stream.getFrame().toImageEx() );
					}
				}
				catch(std::exception& e){
					std::cout << e.what() << std::endl;
				}
			}
			else
				images.addImage( ImageEx::fromFile( cmd.filename() ), -1, -1, cmd.filename() );
		}
		else if( cmd.is( "no-gui" ) ) //Prevent GUI from starting
			use_gui = false;
		else if( cmd.is( "pre-process" ) ){
			auto processor = processingParser( cmd.arguments() );
			for( auto& group : images )
				for( auto& item : group )
					processor->process( item.imageRef() );
		}
		else if( cmd.is( "post-process" ) ){
			int id;
			QString arguments;
			convert( cmd.arguments(), id, arguments );
			
			processingParser( arguments )->process( renders[requireBound( id, 0, renders.size() )] );
		}
		else if( cmd.is( "comparator" ) ){
			comparatorParser( cmd.arguments(), images );
		}
		else if( cmd.is( "align" ) ){
			alignerParser( cmd.arguments(), images );
		}
		else if( cmd.is( "render" ) ){
			renders.push_back( renderParser( cmd.arguments(), images ) );
		}
		else if( cmd.is( "save" ) ){
			Splitter args( cmd.arguments(), ':' );
			auto id = requireBound( asInt( args.left ), 0, renders.size() );
			renders[id].to_qimage().save( args.right );
		}
		else if( cmd.is( "remove-subtitles" ) ){
			std::cout << "Detecting and removing subtitles..." << std::endl;
			
			// 对每张图片检测并移除字幕
			for( auto& group : images ){
				for( auto& item : group ){
					auto& img = item.imageRef();
					
					// 获取灰度图用于字幕检测
					if( img.is_valid() && img.size() > 0 ){
						// 创建灰度副本用于检测
						Plane gray( img[0] );  // 使用第一个通道的副本
						auto region = SubtitleDetector::detect( gray );
						
						if( region.has_subtitle ){
							std::cout << "  Found subtitle at y=" << region.y 
							          << ", height=" << region.height << std::endl;
							
							// 裁剪掉字幕区域
							for( unsigned c = 0; c < img.size(); c++ ){
								Plane& plane = img[c];
								plane.crop( 
									Point<unsigned>(0, 0), 
									Size<unsigned>(plane.get_width(), region.y) 
								);
							}
						}
					}
				}
			}
			
			std::cout << "Subtitle removal complete." << std::endl;
		}
		else if( cmd.is( "scene-detect" ) ){
			// 解析阈值参数
			double static_threshold = 2.0;
			double pan_threshold = 10.0;
			if( !cmd.arguments().isEmpty() ){
				// 格式: static_threshold:pan_threshold
				QStringList thresholds = cmd.arguments().split(':');
				if( thresholds.size() >= 1 ){
					bool ok;
					double val = thresholds[0].toDouble(&ok);
					if( ok && val > 0 ) static_threshold = val;
				}
				if( thresholds.size() >= 2 ){
					bool ok;
					double val = thresholds[1].toDouble(&ok);
					if( ok && val > 0 ) pan_threshold = val;
				}
			}
			
			std::cout << "Analyzing frames..." << std::endl;
			std::cout << "  Static threshold: " << static_threshold << std::endl;
			std::cout << "  Pan threshold: " << pan_threshold << std::endl;
			
			// 收集所有灰度帧
			std::vector<Plane> gray_frames;
			for( auto& group : images ){
				for( auto& item : group ){
					auto& img = item.imageRef();
					if( img.is_valid() && img.size() > 0 ){
						gray_frames.push_back( Plane(img[0]) );
					}
				}
			}
			
			// 分析帧
			auto scenes = SceneDetector::analyzeFrames( gray_frames, static_threshold, pan_threshold );
			
			// 输出每帧信息
			std::cout << "\nFrame analysis:" << std::endl;
			for( size_t i = 0; i < scenes.size(); i++ ){
				const char* type_str = "STATIC";
				if( scenes[i].type == ShotType::PAN ) type_str = "PAN";
				if( scenes[i].type == ShotType::TRANSITION ) type_str = "TRANSITION";
				
				std::cout << "  Frame " << i 
				          << " (score=" << scenes[i].score << ")"
				          << " [" << type_str << "]" << std::endl;
			}
			
			// 分割镜头
			auto shots = SceneDetector::segmentShots( scenes );
			
			// 输出镜头信息
			std::cout << "\nShot segments:" << std::endl;
			for( size_t i = 0; i < shots.size(); i++ ){
				const char* type_str = "STATIC";
				if( shots[i].type == ShotType::PAN ) type_str = "PAN";
				if( shots[i].type == ShotType::TRANSITION ) type_str = "TRANSITION";
				
				std::cout << "  Shot " << i + 1 
				          << ": frame " << shots[i].start_frame 
				          << "-" << shots[i].end_frame
				          << " (" << (shots[i].end_frame - shots[i].start_frame + 1) << " frames)"
				          << " [" << type_str << "]"
				          << " avg_diff=" << shots[i].avg_diff << std::endl;
			}
			
			// 获取平移镜头
			auto longest = SceneDetector::getLongestPanShot( shots );
			if( longest.end_frame > longest.start_frame ){
				std::cout << "\nLongest pan shot: frame " << longest.start_frame 
				          << " to " << longest.end_frame
				          << " (" << (longest.end_frame - longest.start_frame + 1) << " frames)" << std::endl;
				std::cout << "To use this shot, extract frames " 
				          << longest.start_frame << "-" << longest.end_frame << " from the video." << std::endl;
			} else {
				std::cout << "\nNo pan shot detected." << std::endl;
			}
		}
		else if( cmd.is( "help" ) )
			printHelp( cmd.arguments() );
		else{
			qWarning() << "Unknown command:" << cmd.parts.left;
		}
	}
	
}
