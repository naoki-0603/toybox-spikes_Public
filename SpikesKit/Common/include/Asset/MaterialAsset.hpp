// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_MATERIAL_ASSET_HPP_
#define SPIKES_KIT_COMMON_ASSET_MATERIAL_ASSET_HPP_

#include "Asset/AssetTypes.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			enum class MaterialAlphaMode
			{
				Opaque,
				Mask,
				Blend
			};

			struct MaterialAsset final
			{
				struct Parameters
				{
					f32 m_alphaCutoff{};
					bool m_doubleSided{};

					Vector4 m_baseColorFactor{};
					f32 m_metallicFactor{};
					f32 m_roughnessFactor{};

					f32 m_normalScale{ 1.0f };
					f32 m_occlusionStrength{ 1.0f };
					Vector3 m_emissiveFactor{};
				};

			public:
				// TODO: (naoki) 
				// 現在はMVP優先のため文字列を直接保持。
				// 後日 u64 のハッシュ値による String Registry 管理へリファクタリングする
				char m_name[64]{};
				AssetMetadata m_metadata{};

				MaterialAlphaMode m_alphaMode{};

				Parameters m_parameters{};

				ShaderAssetHandle m_shaderAssetHandle{};

				TextureAssetHandle m_baseColorTextureHandle{};
				TextureAssetHandle m_metallicRoughnessTextureHandle{};
				TextureAssetHandle m_normalTextureHandle{};
				TextureAssetHandle m_occlusionTextureHandle{};
				TextureAssetHandle m_emissiveTextureHandle{};

				SamplerResourceHandle m_baseColorTextureSamplerHandle{};
				SamplerResourceHandle m_metallicRoughnessTextureSamplerHandle{};
				SamplerResourceHandle m_normalTextureSamplerHandle{};
				SamplerResourceHandle m_occlusionTextureSamplerHandle{};
				SamplerResourceHandle m_emissiveTextureSamplerHandle{};

				BufferResourceHandle m_constantBufferHandle{};
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_MATERIAL_ASSET_HPP_