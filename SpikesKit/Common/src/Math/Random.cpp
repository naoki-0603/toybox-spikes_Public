// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/Random.hpp"

namespace ts
{
	namespace kit
	{
		namespace math
		{
			RandomResources Random::s_resources = {};

			Random::Random()
			{
			}

			u8 Random::GetByte()
			{
				return s_resources.m_distributionByte(
					s_resources.m_generator
				);
			}
		} // namespace math
	} // namespace kit
} // namespace ts