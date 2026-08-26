// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Importer/TextureImporter.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			TextureImporter::TextureImporter()
			{
			}

			TextureImpoterResult TextureImporter::Import(const TextureImportConfig& config)
			{
				TextureImpoterResult result = {};

				i32 originalChannel = 0;
				i32 width = -1, height = -1;

				stbi_uc* data = stbi_load(
					config.m_path.string().c_str(),
					&width,
					&height,
					&originalChannel,
					STBI_rgb_alpha
				);
				if (!data)
				{
					TS_ASSERT(false, "");
				}

				const u64 sizeInBytes = static_cast<u64>(
					width * height * 4 // (STBI_rgb_alpha)
				);
				result.m_pixels.assign(data, data + sizeInBytes);
				result.m_channel = 4;
				result.m_size.x = width;
				result.m_size.y = height;

				stbi_image_free(data);

				return result;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts