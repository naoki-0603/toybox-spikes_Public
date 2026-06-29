// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/Math.hpp"

using namespace DirectX;

namespace ts
{
	namespace kit
	{
		namespace math
		{
			f32 ToRadians(f32 degrees)
			{
				return XMConvertToRadians(degrees);
			}

			f32 ToDegrees(f32 radians)
			{
				return XMConvertToDegrees(radians);
			}

			bool ApproxEqual(f32 v1, f32 v2, f32 epsilon)
			{
				return ((v2 - epsilon) <= v1 && v1 <= (v2 + epsilon));
			}
		} // namespace math
	} // namespace kit
} // namespace ts