// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/IVector.hpp"

using namespace DirectX;

namespace ts
{
	namespace kit
	{
		namespace math
		{
			IVector2::IVector2() :
				x(), y()
			{
			}

			IVector2::IVector2(i32 x, i32 y) :
				x(x), y(y)
			{
			}

			IVector2 IVector2::operator+() const
			{
				return *this;
			}

			IVector2 IVector2::operator-() const
			{
				return IVector2(-x, -y);
			}

			IVector2 IVector2::operator+(const IVector2& rhs) const
			{
				XMVECTOR lhsXM = internal::IVector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::IVector2ToXMVector(rhs);

				return internal::XMVectorToIVector2(
					XMVectorAdd(lhsXM, rhsXM)
				);
			}

			IVector2 IVector2::operator-(const IVector2& rhs) const
			{
				XMVECTOR lhsXM = internal::IVector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::IVector2ToXMVector(rhs);

				return internal::XMVectorToIVector2(
					XMVectorSubtract(lhsXM, rhsXM)
				);
			}

			IVector2 IVector2::operator*(const IVector2& rhs) const
			{
				XMVECTOR lhsXM = internal::IVector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::IVector2ToXMVector(rhs);

				return internal::XMVectorToIVector2(
					XMVectorMultiply(lhsXM, rhsXM)
				);
			}

			IVector2 IVector2::operator/(const IVector2& rhs) const
			{
				XMVECTOR lhsXM = internal::IVector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::IVector2ToXMVector(rhs);

				return internal::XMVectorToIVector2(
					XMVectorDivide(lhsXM, rhsXM)
				);
			}

			IVector2 IVector2::operator*(i32 scalar) const
			{
				XMVECTOR xmVector = internal::IVector2ToXMVector(*this);

				return internal::XMVectorToIVector2(
					XMVectorScale(xmVector, static_cast<f32>(scalar))
				);
			}

			IVector2 IVector2::operator/(i32 scalar) const
			{
				XMVECTOR lhsXM = internal::IVector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::IVector2ToXMVector(IVector2(scalar, scalar));

				return internal::XMVectorToIVector2(
					XMVectorDivide(lhsXM, rhsXM)
				);
			}

			IVector2& IVector2::operator+=(const IVector2& rhs)
			{
				*this = *this + rhs;
				
				return *this;
			}

			IVector2& IVector2::operator-=(const IVector2& rhs)
			{
				*this = *this - rhs;

				return *this;
			}
			
			IVector2& IVector2::operator*=(i32 scalar)
			{
				*this = *this * scalar;
				
				return *this;
			}
			
			IVector2& IVector2::operator/=(i32 scalar)
			{
				*this = *this / scalar;

				return *this;
			}

			bool IVector2::IsZero() const
			{
				return (x == 0 && y == 0);
			}

			namespace internal
			{
				DirectX::XMVECTOR IVector2ToXMVector(const IVector2& iv)
				{
					const XMINT2 i2(iv.x, iv.y);

					return XMLoadSInt2(&i2);
				}

				IVector2 XMVectorToIVector2(const DirectX::XMVECTOR& iv)
				{
					XMINT2 i2 = {};

					XMStoreSInt2(&i2, iv);

					return IVector2(i2.x, i2.y);
				}
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

ts::kit::math::IVector2 operator*(ts::i32 scalar, const ts::kit::math::IVector2& rhs)
{
	return rhs * scalar;
}