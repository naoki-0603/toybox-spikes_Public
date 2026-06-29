// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_MATH_RANDOM_HPP_
#define SPIKES_KIT_COMMON_MATH_RANDOM_HPP_

namespace ts
{
	namespace kit
	{
		namespace math
		{
			struct RandomResources final
			{
				std::random_device m_randomDevice{};
				std::mt19937 m_generator{ m_randomDevice() };
				std::uniform_int_distribution<u32> m_distributionByte{ 0u, 255u };
			};

			class Random
			{
			public:
				Random();
				Random(const Random&) = delete;
				Random(Random&&) noexcept = delete;
				~Random() noexcept = default;
				
				Random& operator=(const Random&) = delete;
				Random& operator=(Random&&) noexcept = delete;

				/**
				 * @return 0 ~ 255の範囲でランダムに値を返す
				 */
				[[nodiscard]]
				static u8 GetByte();
			private:
				static RandomResources s_resources;
			};
		} // namespace math
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_MATH_RANDOM_HPP_