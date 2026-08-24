// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_MATERIAL_CONSTANTS_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_MATERIAL_CONSTANTS_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct alignas(256) PBRMaterialConstants final
			{
				Vector4 m_baseColorFactor{};

				f32 m_alphaCutoff{};
				i32 m_doubleSided{};
				i32 m_alphaMode{};
				f32 m_metallicFactor{};
				
				f32 m_roughnessFactor{};
				f32 m_normalScale{  };
				f32 m_occlusionStrength{};
				f32 m_padding{};
				
				Vector3 m_emissiveFactor{};
				f32 m_padding2{};

				i32 m_baseColorTextureIndex{ -1 };
				i32 m_baseColorSamplerIndex{ -1 };
				i32 m_baseColorTextureTexcoord{ -1 };

				i32 m_metallicRoughnessTextureIndex{ -1 };
				i32 m_metallicRoughnessSamplerIndex{ -1 };
				i32 m_metallicRoughnessTextureTexcoord{ -1 };

				i32 m_normalTextureIndex{ -1 };
				i32 m_normalTextureSamplerIndex{ -1 };
				i32 m_normalTextureTexcoord{ -1 };

				i32 m_occlusionTextureIndex{ -1 };
				i32 m_occlusionTextureSamplerIndex{ -1 };
				i32 m_occlusionTextureTexcoord{ -1 };

				i32 m_emissiveTextureIndex{ -1 };
				i32 m_emissiveTextureSamplerIndex{ -1 };
				i32 m_emissiveTextureTexcoord{ -1 };

				f32 m_padding3{};
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_MATERIAL_CONSTANTS_HPP_