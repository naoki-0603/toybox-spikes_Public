// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_STATIC_MESH_ASSET_HPP_
#define SPIKES_KIT_COMMON_ASSET_STATIC_MESH_ASSET_HPP_

#include "Asset/AssetTypes.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct StaticMeshAsset final
			{
				struct SubMesh final
				{
					char m_name[64]{};
					BufferResourceHandle m_vertexBufferHandle{};
					BufferResourceHandle m_indexBufferHandle{};
					MaterialAssetHandle m_materialAssetHandle{};
					u32 m_indexCount{};

					Matrix m_worldTransform{};
				};
			public:
				char m_name[64]{};
				AssetMetadata m_metadata{};
				
				std::vector<SubMesh> m_subMeshes{};
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_STATIC_MESH_ASSET_HPP_