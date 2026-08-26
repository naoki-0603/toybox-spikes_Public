// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_BUILDER_MATERIAL_ASSET_BUILDER_HPP_
#define SPIKES_KIT_COMMON_ASSET_BUILDER_MATERIAL_ASSET_BUILDER_HPP_

#include "Asset/MaterialAsset.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct MaterialAssetBuilderDesc final
			{
				std::string m_name{};
				
				MaterialAlphaMode m_alphaMode{};
				MaterialAsset::Parameters m_parameters{};

				ShaderAssetHandle m_shaderAssetHandle{};

				TextureResourceHandle m_baseColorHandle{};
				TextureResourceHandle m_metallicRoughnessHandle{};
				TextureResourceHandle m_normalHandle{};
				TextureResourceHandle m_occlusionHandle{};
				TextureResourceHandle m_emissiveHandle{};

				SamplerResourceHandle m_baseColorSamplerHandle{};
				SamplerResourceHandle m_metallicRoughnessSamplerHandle{};
				SamplerResourceHandle m_normalSamplerHandle{};
				SamplerResourceHandle m_occlusionSamplerHandle{};
				SamplerResourceHandle m_emissiveSamplerHandle{};

				BufferResourceHandle m_constantBufferHandle{};
			};

			class MaterialAssetBuilder final
			{
			public:
				MaterialAssetBuilder();
				~MaterialAssetBuilder() noexcept = default;

			public:
				[[nodiscard]]
				bool Build(const MaterialAssetBuilderDesc& desc, MaterialAsset& materialAsset);
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_BUILDER_MATERIAL_ASSET_BUILDER_HPP_