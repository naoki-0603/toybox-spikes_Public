// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_TEXTURE_ASSET_HPP_
#define SPIKES_KIT_COMMON_ASSET_TEXTURE_ASSET_HPP_

#include "Asset/AssetTypes.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct TextureAsset final
			{
				AssetMetadata m_metadata{};
				TextureResourceHandle m_resourceHandle{};
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_TEXTURE_ASSET_HPP_