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

// 基于 ffmpeg 的 scene_sad 实现
// 参考: https://github.com/FFmpeg/FFmpeg/blob/master/libavfilter/scene_sad.c
double SceneDetector::calculateSAD(const Plane& frame1, const Plane& frame2) {
	unsigned width = std::min(frame1.get_width(), frame2.get_width());
	unsigned height = std::min(frame1.get_height(), frame2.get_height());
	
	uint64_t sad = 0;
	
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			int diff = static_cast<int>(frame1.pixel(Point<unsigned>(x, y))) 
			         - static_cast<int>(frame2.pixel(Point<unsigned>(x, y)));
			sad += std::abs(diff);
		}
	}
	
	return static_cast<double>(sad);
}

double SceneDetector::calculateNormalizedSAD(const Plane& frame1, const Plane& frame2) {
	unsigned width = std::min(frame1.get_width(), frame2.get_width());
	unsigned height = std::min(frame1.get_height(), frame2.get_height());
	
	if (width == 0 || height == 0) return 0.0;
	
	double sad = calculateSAD(frame1, frame2);
	
	// 归一化到 0-1
	// 最大 SAD = width * height * 255 (每个像素最大差值为 255)
	double max_sad = static_cast<double>(width) * height * 255.0;
	
	return sad / max_sad;
}

std::vector<SceneInfo> SceneDetector::detectScenes(
	const std::vector<Plane>& frames,
	double threshold
) {
	std::vector<SceneInfo> results;
	
	if (frames.size() < 2) {
		// 如果只有 1 帧，它就是唯一的一帧
		if (frames.size() == 1) {
			results.push_back({0, 0.0, false});
		}
		return results;
	}
	
	// 第一帧不是转场
	results.push_back({0, 0.0, false});
	
	// 计算每对相邻帧之间的 SAD
	for (size_t i = 1; i < frames.size(); i++) {
		double score = calculateNormalizedSAD(frames[i-1], frames[i]);
		bool is_transition = score > threshold;
		
		results.push_back({
			static_cast<int>(i),
			score,
			is_transition
		});
	}
	
	return results;
}

std::pair<int, int> SceneDetector::getLongestScene(
	const std::vector<SceneInfo>& scenes,
	int total_frames
) {
	if (scenes.empty()) {
		return {0, total_frames - 1};
	}
	
	// 找到所有转场点
	std::vector<int> transitions;
	transitions.push_back(0); // 第一帧
	
	for (const auto& scene : scenes) {
		if (scene.is_transition) {
			transitions.push_back(scene.frame_index);
		}
	}
	transitions.push_back(total_frames); // 最后一帧之后
	
	// 找到最长的场景
	int longest_start = 0;
	int longest_end = total_frames - 1;
	int longest_length = 0;
	
	for (size_t i = 0; i < transitions.size() - 1; i++) {
		int start = transitions[i];
		int end = transitions[i + 1] - 1;
		int length = end - start + 1;
		
		if (length > longest_length) {
			longest_length = length;
			longest_start = start;
			longest_end = end;
		}
	}
	
	return {longest_start, longest_end};
}

std::pair<int, int> SceneDetector::getMiddleScene(
	const std::vector<SceneInfo>& scenes,
	int total_frames
) {
	if (scenes.empty()) {
		return {0, total_frames - 1};
	}
	
	// 找到所有转场点
	std::vector<int> transitions;
	transitions.push_back(0); // 第一帧
	
	for (const auto& scene : scenes) {
		if (scene.is_transition) {
			transitions.push_back(scene.frame_index);
		}
	}
	transitions.push_back(total_frames); // 最后一帧之后
	
	// 找到中间的场景
	int middle_index = (transitions.size() - 1) / 2;
	
	int start = transitions[middle_index];
	int end = (middle_index + 1 < static_cast<int>(transitions.size())) 
	          ? transitions[middle_index + 1] - 1 
	          : total_frames - 1;
	
	return {start, end};
}
