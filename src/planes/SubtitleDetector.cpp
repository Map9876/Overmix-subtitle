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

#include "SubtitleDetector.hpp"

#include <algorithm>
#include <cmath>

using namespace Overmix;

// 简单的自适应阈值实现
static Plane adaptive_threshold(const Plane& gray, int block_size, double C) {
	unsigned width = gray.get_width();
	unsigned height = gray.get_height();
	Plane result(width, height);
	
	int half_block = block_size / 2;
	
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			// 计算局部均值
			double sum = 0;
			int count = 0;
			
			for (int dy = -half_block; dy <= half_block; dy++) {
				for (int dx = -half_block; dx <= half_block; dx++) {
					int nx = x + dx;
					int ny = y + dy;
					if (nx >= 0 && nx < (int)width && ny >= 0 && ny < (int)height) {
						sum += gray.pixel(Point<unsigned>(nx, ny));
						count++;
					}
				}
			}
			
			double mean = sum / count;
			double value = gray.pixel(Point<unsigned>(x, y));
			
			// 二值化
			result.setPixel(Point<unsigned>(x, y), value > (mean - C) ? 255 : 0);
		}
	}
	
	return result;
}

// 形态学闭运算
static Plane morphological_close(const Plane& binary, int kernel_size) {
	unsigned width = binary.get_width();
	unsigned height = binary.get_height();
	int half = kernel_size / 2;
	
	// 膨胀
	Plane dilated(width, height);
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			double max_val = 0;
			for (int dy = -half; dy <= half; dy++) {
				for (int dx = -half; dx <= half; dx++) {
					int nx = x + dx;
					int ny = y + dy;
					if (nx >= 0 && nx < (int)width && ny >= 0 && ny < (int)height) {
						max_val = std::max(max_val, (double)binary.pixel(Point<unsigned>(nx, ny)));
					}
				}
			}
			dilated.setPixel(Point<unsigned>(x, y), max_val);
		}
	}
	
	// 腐蚀
	Plane result(width, height);
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			double min_val = 255;
			for (int dy = -half; dy <= half; dy++) {
				for (int dx = -half; dx <= half; dx++) {
					int nx = x + dx;
					int ny = y + dy;
					if (nx >= 0 && nx < (int)width && ny >= 0 && ny < (int)height) {
						min_val = std::min(min_val, (double)dilated.pixel(Point<unsigned>(nx, ny)));
					}
				}
			}
			result.setPixel(Point<unsigned>(x, y), min_val);
		}
	}
	
	return result;
}

SubtitleRegion SubtitleDetector::detect(const Plane& gray) {
	int width = gray.get_width();
	int height = gray.get_height();
	
	// 参数
	double top_ratio = 0.85;
	double bottom_ratio = 0.99;
	double left_margin_ratio = 0.1;
	double right_margin_ratio = 0.9;
	int kernel_size = 5;
	
	// 自适应阈值化
	Plane mask = adaptive_threshold(gray, 11, 2);
	
	// 形态学闭运算
	mask = morphological_close(mask, kernel_size);
	
	// 定义字幕区域
	int top_region = static_cast<int>(top_ratio * height);
	int bottom_region = static_cast<int>(bottom_ratio * height);
	int left_margin = static_cast<int>(left_margin_ratio * width);
	int right_margin = static_cast<int>(right_margin_ratio * width);
	
	// 检测字幕区域（简化版：统计白色像素）
	int white_count = 0;
	int total_count = 0;
	
	for (int y = top_region; y < bottom_region; y++) {
		for (int x = left_margin; x < right_margin; x++) {
			if (mask.pixel(Point<unsigned>(x, y)) > 128) {
				white_count++;
			}
			total_count++;
		}
	}
	
	// 计算白色像素比例
	double white_ratio = total_count > 0 ? static_cast<double>(white_count) / total_count : 0;
	
	// 判断是否有字幕
	bool has_subtitle = white_ratio > 0.05;
	
	// 找到字幕的顶部边界
	int subtitle_y = top_region;
	if (has_subtitle) {
		for (int y = bottom_region - 1; y >= top_region; y--) {
			int row_white = 0;
			for (int x = left_margin; x < right_margin; x++) {
				if (mask.pixel(Point<unsigned>(x, y)) > 128) {
					row_white++;
				}
			}
			double row_ratio = static_cast<double>(row_white) / (right_margin - left_margin);
			if (row_ratio < 0.02) {
				subtitle_y = y + 1;
				break;
			}
		}
	}
	
	return SubtitleRegion{
		0,
		subtitle_y,
		width,
		height - subtitle_y,
		has_subtitle
	};
}

std::vector<SubtitleRegion> SubtitleDetector::detect_batch(const std::vector<Plane>& grays) {
	std::vector<SubtitleRegion> regions;
	regions.reserve(grays.size());
	
	for (const auto& gray : grays) {
		regions.push_back(detect(gray));
	}
	
	return regions;
}

Plane SubtitleDetector::remove_subtitle(const Plane& plane, const SubtitleRegion& region) {
	if (!region.has_subtitle) {
		return Plane(plane);
	}
	
	// 裁剪掉字幕区域
	unsigned new_height = region.y;
	if (new_height <= 0) {
		return Plane(plane);
	}
	
	Plane result(plane);
	result.crop(Point<unsigned>(0, 0), Size<unsigned>(plane.get_width(), new_height));
	return result;
}
