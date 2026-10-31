// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_HELPER_MESH_HELPER_HPP_
#define SPIKES_KIT_COMMON_HELPER_MESH_HELPER_HPP_

namespace ts
{
	namespace kit
	{
		namespace meshhelper
		{
			struct ComputeTangentsAndBitangentsParameters final
			{
				std::vector<Vector3> m_positions{};
				std::vector<Vector3> m_normals{};
				std::vector<Vector2> m_uvs{};
				std::vector<u8> m_indicesU8{};
				std::vector<u16> m_indicesU16{};
				std::vector<u32> m_indicesU32{};
			};

			[[nodiscard]]
			bool ComputeTangentsAndBitangents(
				const ComputeTangentsAndBitangentsParameters& parameters,
				std::vector<Vector4>& outTangents
			);

			[[nodiscard]]
			f32 CalculateTangentSign(
				const Vector3& normal,
				const Vector3& tangent,
				const Vector3& bitangent
			);

			
		} // namespace meshhelper
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_HELPER_MESH_HELPER_HPP_