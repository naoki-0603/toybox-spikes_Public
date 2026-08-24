// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_ASSET_IMPORTER_TEXTURE_IMPORTER_HPP_
#define SPIKES_KIT_COMMON_CORE_ASSET_IMPORTER_TEXTURE_IMPORTER_HPP_

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			struct TextureImportConfig final
			{
				fs::path m_path{};
			};

			struct TextureImpoterResult final
			{
			public:
				std::vector<u8> m_pixels{};

				IVector2 m_size{};
				i32 m_channel{};
				u32 m_mipLevels{ 1u };
			};

			class TextureImporter final
			{
			public:
				TextureImporter();
				TextureImporter(const TextureImporter&) = delete;
				TextureImporter(TextureImporter&&) noexcept = default;
				~TextureImporter() noexcept = default;
			public:
				[[nodiscard]]
				TextureImpoterResult Import(const TextureImportConfig& config);
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_ASSET_IMPORTER_TEXTURE_IMPORTER_HPP_