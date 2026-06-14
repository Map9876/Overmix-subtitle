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

#ifndef SUBTITLE_DETECTOR_HPP
#define SUBTITLE_DETECTOR_HPP

#include <vector>
#include "Plane.hpp"

namespace Overmix {

struct SubtitleRegion {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	bool has_subtitle = false;
};

class SubtitleDetector {
public:
	/**
	 * 检测图像中的字幕区域
	 * 
	 * 使用自适应阈值化和形态学操作检测字幕
	 * 基于 Video-Subtitles-Detector 项目的方法
	 * 
	 * @param gray 灰度图像
	 * @return 字幕区域信息
	 */
	static SubtitleRegion detect(const Plane& gray);
	
	/**
	 * 批量检测多张图像
	 */
	static std::vector<SubtitleRegion> detect_batch(const std::vector<Plane>& grays);
	
	/**
	 * 裁剪掉字幕区域
	 */
	static Plane remove_subtitle(const Plane& plane, const SubtitleRegion& region);
};

} // namespace Overmix

#endif // SUBTITLE_DETECTOR_HPP
