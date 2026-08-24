// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_STATIC_MESH_VERTEX_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_STATIC_MESH_VERTEX_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct StaticMeshVertex final
			{
				Vector4 m_position{};
				Vector4 m_normal{};
				Vector4 m_tangent{};

				std::array<Vector2, 2u> m_texcoords{};
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_STATIC_MESH_VERTEX_HPP_