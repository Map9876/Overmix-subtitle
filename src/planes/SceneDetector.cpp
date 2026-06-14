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

#include "SceneDetector.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace Overmix;

double SceneDetector::calculateMAD(const Plane& frame1, const Plane& frame2) {
	unsigned width = std::min(frame1.get_width(), frame2.get_width());
	unsigned height = std::min(frame1.get_height(), frame2.get_height());
	
	if (width == 0 || height == 0) return 0.0;
	
	double sum = 0;
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			int diff = static_cast<int>(frame1.pixel(Point<unsigned>(x, y))) 
			         - static_cast<int>(frame2.pixel(Point<unsigned>(x, y)));
			sum += std::abs(diff);
		}
	}
	
	return sum / (width * height);
}

std::vector<SceneInfo> SceneDetector::analyzeFrames(
	const std::vector<Plane>& frames,
	double static_threshold,
	double pan_threshold
) {
	std::vector<SceneInfo> results;
	
	if (frames.empty()) return results;
	
	// 第一帧默认为静止
	results.push_back({0, 0.0, ShotType::STATIC});
	
	// 计算每对相邻帧之间的差异
	for (size_t i = 1; i < frames.size(); i++) {
		double mad = calculateMAD(frames[i-1], frames[i]);
		
		ShotType type;
		if (mad < static_threshold) {
			type = ShotType::STATIC;
		} else if (mad < pan_threshold) {
			type = ShotType::PAN;
		} else {
			type = ShotType::TRANSITION;
		}
		
		results.push_back({
			static_cast<int>(i),
			mad,
			type
		});
	}
	
	return results;
}

std::vector<ShotInfo> SceneDetector::segmentShots(
	const std::vector<SceneInfo>& scenes,
	int min_shot_length
) {
	std::vector<ShotInfo> shots;
	
	if (scenes.empty()) return shots;
	
	// 开始第一个镜头
	int start = 0;
	ShotType current_type = scenes[0].type;
	double sum_diff = 0;
	int count = 0;
	
	for (size_t i = 1; i < scenes.size(); i++) {
		if (scenes[i].type != current_type) {
			// 镜头类型变化，保存当前镜头
			int length = i - start;
			if (length >= min_shot_length) {
				shots.push_back({
					start,
					static_cast<int>(i - 1),
					current_type,
					count > 0 ? sum_diff / count : 0
				});
			}
			
			// 开始新镜头
			start = i;
			current_type = scenes[i].type;
			sum_diff = 0;
			count = 0;
		}
		
		sum_diff += scenes[i].score;
		count++;
	}
	
	// 保存最后一个镜头
	int length = scenes.size() - start;
	if (length >= min_shot_length) {
		shots.push_back({
			start,
			static_cast<int>(scenes.size() - 1),
			current_type,
			count > 0 ? sum_diff / count : 0
		});
	}
	
	return shots;
}

ShotInfo SceneDetector::getLongestPanShot(const std::vector<ShotInfo>& shots) {
	ShotInfo longest;
	int max_length = 0;
	
	for (const auto& shot : shots) {
		if (shot.type == ShotType::PAN) {
			int length = shot.end_frame - shot.start_frame + 1;
			if (length > max_length) {
				max_length = length;
				longest = shot;
			}
		}
	}
	
	return longest;
}

std::vector<ShotInfo> SceneDetector::getPanShots(const std::vector<ShotInfo>& shots) {
	std::vector<ShotInfo> pan_shots;
	for (const auto& shot : shots) {
		if (shot.type == ShotType::PAN) {
			pan_shots.push_back(shot);
		}
	}
	return pan_shots;
}

std::vector<ShotInfo> SceneDetector::getStaticShots(const std::vector<ShotInfo>& shots) {
	std::vector<ShotInfo> static_shots;
	for (const auto& shot : shots) {
		if (shot.type == ShotType::STATIC) {
			static_shots.push_back(shot);
		}
	}
	return static_shots;
}
