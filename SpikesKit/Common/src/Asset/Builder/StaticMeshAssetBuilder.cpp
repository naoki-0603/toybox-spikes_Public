// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Builder/StaticMeshAssetBuilder.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			StaticMeshAssetBuilder::StaticMeshAssetBuilder()
			{
			}

			bool StaticMeshAssetBuilder::Build(
				const StaticMeshAssetBuilderDesc& desc,
				StaticMeshAsset& staticMeshAsset
			)
			{
				if (desc.m_name.size() > 64)
				{
					TS_ASSERT(false, "メッシュ名が長すぎます。");

					return false;
				}
				std::memcpy(staticMeshAsset.m_name, desc.m_name.data(), desc.m_name.size());

				staticMeshAsset.m_subMeshes = desc.m_subMeshes;

				return true;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts