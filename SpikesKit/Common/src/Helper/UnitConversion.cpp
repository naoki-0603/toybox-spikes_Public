// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Helper/UnitConversion.hpp"

namespace ts
{
	namespace kit
	{
		u64 FormatBytes(u64 sizeInBytes) noexcept
		{
			if (sizeInBytes >= math::k_unitGByte)
			{
				return sizeInBytes / math::k_unitGByte;
			}
			else if (sizeInBytes >= math::k_unitMByte)
			{
				return sizeInBytes / math::k_unitMByte;
			}
			else if (sizeInBytes >= math::k_unitKByte)
			{
				return sizeInBytes / math::k_unitKByte;
			}

			return sizeInBytes;
		}

		u64 ToKB(u64 sizeInBytes) noexcept
		{
			return sizeInBytes / 1024u;
		}

		u64 ToMB(u64 sizeInKeroBytes) noexcept
		{
			return sizeInKeroBytes / 1024u;
		}

		u64 ToGB(u64 sizeInMegaBytes) noexcept
		{
			return sizeInMegaBytes / 1024u;
		}
	} // namespace kit
} // namespace ts