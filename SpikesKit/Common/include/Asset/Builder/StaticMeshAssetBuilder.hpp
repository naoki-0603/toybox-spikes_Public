// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_BUILDER_STATIC_MESH_ASSET_BUILDER_HPP_
#define SPIKES_KIT_COMMON_ASSET_BUILDER_STATIC_MESH_ASSET_BUILDER_HPP_

#include "Asset/StaticMeshAsset.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct StaticMeshAssetBuilderDesc final
			{
				std::string m_name{};
				std::vector<StaticMeshAsset::SubMesh> m_subMeshes{};
			};

			class StaticMeshAssetBuilder final
			{
			public:
				StaticMeshAssetBuilder();
				~StaticMeshAssetBuilder() noexcept = default;
			
			public:
				[[nodiscard]]
				bool Build(const StaticMeshAssetBuilderDesc& desc, StaticMeshAsset& staticMeshAsset);
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_BUILDER_STATIC_MESH_ASSET_BUILDER_HPP_