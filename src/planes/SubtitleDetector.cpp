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
	double check_ratio = 0.25; // 检查底部 25% 的区域
	int check_height = static_cast<int>(check_ratio * height);
	int start_y = height - check_height;
	
	// 从底部向上扫描，找字幕区域
	// 字幕特征：
	// 1. 黑底白字：深色背景条 + 白色文字
	// 2. 白底深字：白色背景上的深色文字块（非白色区域聚集）
	
	int subtitle_top = -1;
	
	// 逐行扫描底部区域
	for (int y = start_y; y < height; y++) {
		// 计算这一行的统计信息
		int dark_count = 0;   // 深色像素数
		int bright_count = 0; // 亮色像素数
		int non_white_count = 0; // 非白色像素数
		
		for (int x = 0; x < width; x++) {
			double val = gray.pixel(Point<unsigned>(x, y));
			// 注意：像素值范围是 0-16383（不是 0-255）
			// 8位的 80 ≈ 5140, 8位的 200 ≈ 12850
			if (val < 5140) dark_count++;     // 8位 < 80
			if (val > 12850) bright_count++;  // 8位 > 200
			if (val < 12850) non_white_count++; // 8位 < 200
		}
		
		double dark_ratio = static_cast<double>(dark_count) / width;
		double bright_ratio = static_cast<double>(bright_count) / width;
		double non_white_ratio = static_cast<double>(non_white_count) / width;
		
		// 字幕行特征（两种类型）：
		// 类型1 - 黑底白字：深色像素 > 30%，亮色像素 > 3%
		// 类型2 - 白底深字：非白色像素 > 10% 且 < 80%（排除大面积深色区域）
		bool is_subtitle_row = false;
		
		if (dark_ratio > 0.3 && bright_ratio > 0.03) {
			// 黑底白字
			is_subtitle_row = true;
		} else if (non_white_ratio > 0.10 && non_white_ratio < 0.80) {
			// 白底深字（字幕文字区域）
			// 额外检查：非白色像素应该聚集在某个区域（字幕通常在中间或底部）
			// 计算非白色像素的分布
			int first_non_white = -1;
			int last_non_white = -1;
			for (int x = 0; x < width; x++) {
				if (gray.pixel(Point<unsigned>(x, y)) < 12850) { // 8位 < 200
					if (first_non_white == -1) first_non_white = x;
					last_non_white = x;
				}
			}
			// 字幕通常不会占据整行宽度，而是集中在某个区域
			if (first_non_white >= 0) {
				int span = last_non_white - first_non_white;
				if (span < width * 0.85) {
					is_subtitle_row = true;
				}
			}
		}
		
		if (is_subtitle_row && subtitle_top == -1) {
			subtitle_top = y;
		}
	}
	
	// 如果没有找到字幕行，返回没有字幕
	if (subtitle_top == -1) {
		return SubtitleRegion{0, 0, width, 0, false};
	}
	
	// 找到字幕区域的顶部（从 subtitle_top 向上找字幕开始的地方）
	int final_top = subtitle_top;
	for (int y = subtitle_top - 1; y >= start_y; y--) {
		int dark_count = 0;
		int bright_count = 0;
		int non_white_count = 0;
		
		for (int x = 0; x < width; x++) {
			double val = gray.pixel(Point<unsigned>(x, y));
			if (val < 80) dark_count++;
			if (val > 200) bright_count++;
			if (val < 200) non_white_count++;
		}
		
		double dark_ratio = static_cast<double>(dark_count) / width;
		double bright_ratio = static_cast<double>(bright_count) / width;
		double non_white_ratio = static_cast<double>(non_white_count) / width;
		
		bool is_subtitle_row = false;
		if (dark_ratio > 0.3 && bright_ratio > 0.03) {
			is_subtitle_row = true;
		} else if (non_white_ratio > 0.10 && non_white_ratio < 0.80) {
			int first_non_white = width;
			int last_non_white = 0;
			for (int x = 0; x < width; x++) {
				if (gray.pixel(Point<unsigned>(x, y)) < 12850) { // 8位 < 200
					if (x < first_non_white) first_non_white = x;
					if (x > last_non_white) last_non_white = x;
				}
			}
			int span = last_non_white - first_non_white;
			if (span > 0 && span < width * 0.6) {
				is_subtitle_row = true;
			}
		}
		
		if (is_subtitle_row) {
			final_top = y;
		} else {
			break; // 不是字幕行，停止
		}
	}
	
	return SubtitleRegion{
		0,
		final_top,
		width,
		height - final_top,
		true
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
