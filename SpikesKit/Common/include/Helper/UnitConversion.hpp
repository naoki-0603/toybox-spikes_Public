// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_HELPER_UNIT_CONVERSION_HPP_
#define SPIKES_KIT_COMMON_HELPER_UNIT_CONVERSION_HPP_

namespace ts
{
	namespace kit
	{
		/// 引数で与えられたサイズを適切な単位に変換する
		[[nodiscard]]
		u64 FormatBytes(u64 sizeInBytes) noexcept;

		[[nodiscard]]
		u64 ToKB(u64 sizeInBytes) noexcept;

		[[nodiscard]]
		u64 ToMB(u64 sizeInKeroBytes) noexcept;

		[[nodiscarrd]]
		u64 ToGB(u64 sizeInMegaBytes) noexcept;
	} // namespace kit
} // namespace ts

#endif // !SPIKES_KIT_COMMON_HELPER_UNIT_CONVERSION_HPP_