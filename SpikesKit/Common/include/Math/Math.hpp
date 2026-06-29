// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_MATH_MATH_HPP_
#define SPIKES_KIT_COMMON_MATH_MATH_HPP_

namespace ts
{
	namespace kit
	{
		namespace math
		{
			/// 数学用の便利な定数
			constexpr f32 k_pi = DirectX::XM_PI; /// π
			constexpr f32 k_pi2 = DirectX::XM_2PI; /// 2π 
			constexpr f32 k_1divpi = DirectX::XM_1DIVPI; /// 1 / π
			constexpr f32 k_1div2pi = DirectX::XM_1DIV2PI; /// 2 / π
			constexpr f32 k_pidiv2 = DirectX::XM_PIDIV2; /// π / 2
			constexpr f32 k_pidiv4 = DirectX::XM_PIDIV4; /// π / 4
			constexpr f32 k_epsilon = 1.192092896e-7f;

			constexpr u64 k_unitByte = 1u; // In byte
			constexpr u64 k_unitKByte = 1024u; // In byte
			constexpr u64 k_unitMByte = 1024u * 1024u; // In byte
			constexpr u64 k_unitGByte = 1024u * 1024u * 1024u; // In byte

			/// 数学用の便利な関数
			[[nodiscard]]
			f32 ToRadians(f32 degrees);

			[[nodiscard]]
			f32 ToDegrees(f32 radians);

			/*
			* @details ((v2 - epsilon) <= v1 && v1 <= (v2 + epsilon))
			*/
			[[nodiscard]]
			bool ApproxEqual(f32 v1, f32 v2, f32 epsilon = k_epsilon);
		} // namespace math
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_MATH_MATH_HPP_