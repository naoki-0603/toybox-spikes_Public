// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_SCENE_CONSTANTS_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_SCENE_CONSTANTS_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct alignas(256) SceneConstants final
			{
				Matrix m_viewProjection{};
				Vector4 m_cameraPosition{};

				f32 m_deltaTime{};
				f32 m_totalTime{};
				Vector2 m_padding{};
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_SCENE_CONSTANTS_HPP_