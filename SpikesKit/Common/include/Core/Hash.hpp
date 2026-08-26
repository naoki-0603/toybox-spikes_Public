// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_HASH_HPP_
#define SPIKES_KIT_COMMON_CORE_HASH_HPP_

namespace ts
{
	namespace kit
	{
		namespace hash
		{
			template <class T>
			inline void HashCombine(uint64_t& seed, const T& v)
			{
				std::hash<T> hasher;

				// 0x9e3779b97f4a7c15 は64ビットのマジックナンバー
				seed ^= hasher(v) + 0x9e3779b97f4a7c15ULL + (seed << 12) + (seed >> 4);
			}
		} // namespace hash
	} // namespace ts
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_HASH_HPP_