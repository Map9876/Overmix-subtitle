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

#ifndef SCENE_DETECTOR_HPP
#define SCENE_DETECTOR_HPP

#include <vector>
#include "Plane.hpp"

namespace Overmix {

struct SceneInfo {
	int frame_index = 0;      // 帧索引
	double score = 0.0;       // 转场分数 (0-1)
	bool is_transition = false; // 是否是转场帧
};

class SceneDetector {
public:
	/**
	 * 计算两帧之间的 SAD (Sum of Absolute Differences)
	 * 基于 ffmpeg 的 scene_sad 实现
	 * 
	 * @param frame1 第一帧灰度图
	 * @param frame2 第二帧灰度图
	 * @return SAD 值
	 */
	static double calculateSAD(const Plane& frame1, const Plane& frame2);
	
	/**
	 * 计算两帧之间的归一化 SAD (0-1)
	 */
	static double calculateNormalizedSAD(const Plane& frame1, const Plane& frame2);
	
	/**
	 * 检测视频帧序列中的转场
	 * 
	 * @param frames 灰度帧序列
	 * @param threshold 转场阈值 (默认 0.3)
	 * @return 每帧的转场信息
	 */
	static std::vector<SceneInfo> detectScenes(
		const std::vector<Plane>& frames,
		double threshold = 0.3
	);
	
	/**
	 * 获取最长连续场景的帧范围
	 * 
	 * @param scenes 转场信息
	 * @param total_frames 总帧数
	 * @return (起始帧, 结束帧)
	 */
	static std::pair<int, int> getLongestScene(
		const std::vector<SceneInfo>& scenes,
		int total_frames
	);
	
	/**
	 * 获取中间场景的帧范围
	 * 
	 * @param scenes 转场信息
	 * @param total_frames 总帧数
	 * @return (起始帧, 结束帧)
	 */
	static std::pair<int, int> getMiddleScene(
		const std::vector<SceneInfo>& scenes,
		int total_frames
	);
};

} // namespace Overmix

#endif // SCENE_DETECTOR_HPP
