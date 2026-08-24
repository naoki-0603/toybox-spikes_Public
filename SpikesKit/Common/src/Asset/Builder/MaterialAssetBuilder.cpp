// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Builder/MaterialAssetBuilder.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			MaterialAssetBuilder::MaterialAssetBuilder()
			{
			}

			bool MaterialAssetBuilder::Build(
				const MaterialAssetBuilderDesc& desc,
				MaterialAsset& materialAsset
			)
			{
				if (desc.m_name.size() > 64)
				{
					TS_ASSERT(false, "マテリアル名が長すぎます。");

					return false;
				}
				std::memcpy(materialAsset.m_name, desc.m_name.data(), desc.m_name.size());

				materialAsset.m_alphaMode = desc.m_alphaMode;
				materialAsset.m_parameters = desc.m_parameters;
				
				materialAsset.m_shaderAssetHandle = desc.m_shaderAssetHandle;
				materialAsset.m_constantBufferHandle = desc.m_constantBufferHandle;

				materialAsset.m_baseColorTextureHandle = desc.m_baseColorHandle;
				materialAsset.m_metallicRoughnessTextureHandle = desc.m_metallicRoughnessHandle;
				materialAsset.m_normalTextureHandle = desc.m_normalHandle;
				materialAsset.m_occlusionTextureHandle = desc.m_occlusionHandle;
				materialAsset.m_emissiveTextureHandle = desc.m_emissiveHandle;

				materialAsset.m_baseColorTextureSamplerHandle = desc.m_baseColorSamplerHandle;
				materialAsset.m_metallicRoughnessTextureSamplerHandle = desc.m_metallicRoughnessSamplerHandle;
				materialAsset.m_normalTextureSamplerHandle = desc.m_normalSamplerHandle;
				materialAsset.m_occlusionTextureSamplerHandle = desc.m_occlusionSamplerHandle;
				materialAsset.m_emissiveTextureSamplerHandle = desc.m_emissiveSamplerHandle;

				return true;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts