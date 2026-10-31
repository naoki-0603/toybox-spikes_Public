// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_VERTEX_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_VERTEX_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct DebugVertex final
			{
				Vector3 m_position{};
				u32 m_color{ 0xFFFFFFFF }; // Packed RGBA 
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_VERTEX_HPP_