// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/UUID.hpp"
#include "Math/Random.hpp"

namespace ts
{
	namespace kit
	{
		namespace math
		{
			UUID::UUID() :
				m_uuid(), m_generated()
			{
			}

			bool UUID::operator==(const UUID& rhs) const
			{
				return (m_uuid == rhs.m_uuid);
			}

			bool UUID::operator!=(const UUID& rhs) const
			{
				return !(*this == rhs);
			}

			void UUID::Generate()
			{
				std::array<u8, 16u> bytes = {};
				for (u32 i = 0u; i < static_cast<u32>(bytes.size()); ++i)
				{
					bytes[i] = Random::GetByte();
				}

				// UUIDv4 上位4ビット目を強制的に1にする
				bytes[6] = (bytes[6] & 0x0F); // 上位4ビットを強制的に 0 にする
				bytes[6] = bytes[6] | 0x40; // 上位4ビットを強制的に 1 にする

				// UUIDv4 上位2ビット目を強制的に1にする
				bytes[8] = (bytes[8] & 0x3F); // 上位2ビットを強制的に 0 にする
				bytes[8] = bytes[8] | 0x80; // 上位2ビットを 10 に設定する

				m_uuid = std::format(
					"{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
					bytes[0], bytes[1], bytes[2], bytes[3],
					bytes[4], bytes[5],
					bytes[6], bytes[7],
					bytes[8], bytes[9], 
					bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]
				);

				m_generated = true;
			}

			const std::string& UUID::GetUUIDChecked() const
			{
				TS_ASSERT(m_generated, "UUIDが生成されていません。");

				return m_uuid;
			}

			UUID GenerateUUID()
			{
				UUID uuid = {};
				uuid.Generate();

				return uuid;
			}
		} // namespace math
	} // namespace kit
} // namespace ts