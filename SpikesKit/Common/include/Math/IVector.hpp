// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_MATH_IVECTOR_HPP_
#define SPIKES_KIT_COMMON_CORE_MATH_IVECTOR_HPP_

namespace ts
{
	namespace kit
	{
		namespace math
		{
			struct IVector2
			{
				IVector2();
				IVector2(i32 x, i32 y);
				IVector2(const IVector2&) = default;
				IVector2(IVector2&&) noexcept = default;

				IVector2& operator=(const IVector2&) = default;
				IVector2& operator=(IVector2&&) noexcept = default;

				IVector2 operator+() const;
				IVector2 operator-() const;
				IVector2 operator+(const IVector2& rhs) const;
				IVector2 operator-(const IVector2& rhs) const;
				IVector2 operator*(const IVector2& rhs) const;
				IVector2 operator/(const IVector2& rhs) const;
				IVector2 operator*(i32 scalar) const;
				IVector2 operator/(i32 scalar) const;

				IVector2& operator+=(const IVector2& rhs);
				IVector2& operator-=(const IVector2& rhs);
				IVector2& operator*=(i32 scalar);
				IVector2& operator/=(i32 scalar);

				[[nodiscard]]
				bool IsZero() const;

				i32 x, y;
			};

			namespace internal
			{
				[[nodiscard]]
				DirectX::XMVECTOR IVector2ToXMVector(const IVector2& iv);

				[[nodiscard]]
				IVector2 XMVectorToIVector2(const DirectX::XMVECTOR& iv);
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

ts::kit::math::IVector2 operator*(ts::i32 scalar, const ts::kit::math::IVector2& rhs);

#endif //! SPIKES_KIT_COMMON_CORE_MATH_IVECTOR_HPP_