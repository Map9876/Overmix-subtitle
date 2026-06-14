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

// 镜头类型
enum class ShotType {
	STATIC,    // 静止镜头
	PAN,       // 平移镜头
	TRANSITION // 转场
};

struct SceneInfo {
	int frame_index = 0;      // 帧索引
	double score = 0.0;       // 帧间差异分数
	ShotType type = ShotType::STATIC; // 镜头类型
};

struct ShotInfo {
	int start_frame = 0;      // 起始帧
	int end_frame = 0;        // 结束帧
	ShotType type = ShotType::STATIC; // 镜头类型
	double avg_diff = 0.0;    // 平均帧间差异
};

class SceneDetector {
public:
	/**
	 * 计算两帧之间的平均绝对差异 (MAD)
	 * 
	 * @param frame1 第一帧灰度图
	 * @param frame2 第二帧灰度图
	 * @return MAD 值
	 */
	static double calculateMAD(const Plane& frame1, const Plane& frame2);
	
	/**
	 * 分析视频帧序列，检测镜头类型
	 * 
	 * @param frames 灰度帧序列
	 * @param static_threshold 静止阈值 (默认 2.0)
	 * @param pan_threshold 平移阈值 (默认 10.0)
	 * @return 每帧的场景信息
	 */
	static std::vector<SceneInfo> analyzeFrames(
		const std::vector<Plane>& frames,
		double static_threshold = 2.0,
		double pan_threshold = 10.0
	);
	
	/**
	 * 将帧序列分割为不同的镜头
	 * 
	 * @param scenes 帧分析结果
	 * @param min_shot_length 最小镜头长度 (帧数)
	 * @return 镜头列表
	 */
	static std::vector<ShotInfo> segmentShots(
		const std::vector<SceneInfo>& scenes,
		int min_shot_length = 5
	);
	
	/**
	 * 获取最长的平移镜头
	 * 
	 * @param shots 镜头列表
	 * @return 平移镜头信息，如果没有找到返回空
	 */
	static ShotInfo getLongestPanShot(const std::vector<ShotInfo>& shots);
	
	/**
	 * 获取所有平移镜头
	 * 
	 * @param shots 镜头列表
	 * @return 平移镜头列表
	 */
	static std::vector<ShotInfo> getPanShots(const std::vector<ShotInfo>& shots);
	
	/**
	 * 获取所有静止镜头
	 * 
	 * @param shots 镜头列表
	 * @return 静止镜头列表
	 */
	static std::vector<ShotInfo> getStaticShots(const std::vector<ShotInfo>& shots);
};

} // namespace Overmix

#endif // SCENE_DETECTOR_HPP
