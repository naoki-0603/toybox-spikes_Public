// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/Vector.hpp"
#include "Math/Math.hpp"

using namespace DirectX;

namespace ts
{
	namespace kit
	{
		namespace math
		{
			Vector2::Vector2() :
				x(), y()
			{
			}

			Vector2::Vector2(f32 x, f32 y) :
				x(x), y(y)
			{
			}

			Vector2 Vector2::operator+() const
			{
				return *this;
			}

			Vector2 Vector2::operator-() const
			{
				return Vector2(-x, -y);
			}

			Vector2 Vector2::operator+(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return internal::XMVectorToVector2(
					XMVectorAdd(lhsXM, rhsXM)
				);
			}

			Vector2 Vector2::operator-(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return internal::XMVectorToVector2(
					XMVectorSubtract(lhsXM, rhsXM)
				);
			}
			
			Vector2 Vector2::operator*(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return internal::XMVectorToVector2(
					XMVectorMultiply(lhsXM, rhsXM)
				);
			}

			Vector2 Vector2::operator/(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return internal::XMVectorToVector2(
					XMVectorDivide(lhsXM, rhsXM)
				);
			}

			Vector2 Vector2::operator*(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);

				return internal::XMVectorToVector2(
					XMVectorScale(selfXM, scalar)
				);
			}

			Vector2 Vector2::operator/(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector2ToXMVector(Vector2(scalar, scalar));

				return internal::XMVectorToVector2(
					XMVectorDivide(selfXM, scalarXM)
				);
			}

			Vector2& Vector2::operator+=(const Vector2& rhs)
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				*this = internal::XMVectorToVector2(
					XMVectorAdd(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector2& Vector2::operator-=(const Vector2& rhs)
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				*this = internal::XMVectorToVector2(
					XMVectorSubtract(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector2& Vector2::operator*=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);

				*this = internal::XMVectorToVector2(
					XMVectorScale(selfXM, scalar)
				);

				return *this;
			}

			Vector2& Vector2::operator/=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector2ToXMVector(Vector2(scalar, scalar));

				*this = internal::XMVectorToVector2(
					XMVectorDivide(selfXM, scalarXM)
				);

				return *this;
			}

			f32 Vector2::Length() const
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);

				return XMVectorGetX(XMVector2Length(selfXM));
			}

			f32 Vector2::LengthSq() const
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);

				return XMVectorGetX(XMVector2LengthSq(selfXM));
			}

			f32 Vector2::Cross(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return XMVectorGetX(XMVector2Cross(lhsXM, rhsXM));
			}

			f32 Vector2::Dot(const Vector2& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector2ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector2ToXMVector(rhs);

				return XMVectorGetX(XMVector2Dot(lhsXM, rhsXM));
			}

			bool Vector2::IsNearlyZero() const
			{
				return IsApproxEqual(Vector2());
			}

			bool Vector2::IsApproxEqual(const Vector2& rhs) const
			{
				const bool xApproxEqual = math::ApproxEqual(x, rhs.x);
				const bool yApproxEqual = math::ApproxEqual(y, rhs.y);

				return (xApproxEqual && yApproxEqual);
			}

			void Vector2::Normalize()
			{
				XMVECTOR selfXM = internal::Vector2ToXMVector(*this);

				*this = internal::XMVectorToVector2(
					XMVector2Normalize(selfXM)
				);
			}

			Vector3::Vector3() :
				x(), y(), z()
			{
			}

			Vector3::Vector3(const Vector2& v2, f32 z) :
				x(v2.x), y(v2.y), z(z)
			{
			}

			Vector3::Vector3(f32 x, f32 y, f32 z) :
				x(x), y(y), z(z)
			{
			}

			Vector3 Vector3::operator+() const
			{
				return *this;
			}

			Vector3 Vector3::operator-() const
			{
				return Vector3(-x, -y, -z);
			}

			Vector3 Vector3::operator+(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return internal::XMVectorToVector3(
					XMVectorAdd(lhsXM, rhsXM)
				);
			}

			Vector3 Vector3::operator-(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return internal::XMVectorToVector3(
					XMVectorSubtract(lhsXM, rhsXM)
				);
			}

			Vector3 Vector3::operator*(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return internal::XMVectorToVector3(
					XMVectorMultiply(lhsXM, rhsXM)
				);
			}

			Vector3 Vector3::operator/(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return internal::XMVectorToVector3(
					XMVectorDivide(lhsXM, rhsXM)
				);
			}

			Vector3 Vector3::operator*(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);

				return internal::XMVectorToVector3(
					XMVectorScale(selfXM, scalar)
				);
			}

			Vector3 Vector3::operator/(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector3ToXMVector(Vector3(scalar, scalar, scalar));

				return internal::XMVectorToVector3(
					XMVectorDivide(selfXM, scalarXM)
				);
			}

			Vector3& Vector3::operator+=(const Vector3& rhs)
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				*this = internal::XMVectorToVector3(
					XMVectorAdd(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector3& Vector3::operator-=(const Vector3& rhs)
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				*this = internal::XMVectorToVector3(
					XMVectorSubtract(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector3& Vector3::operator*=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);

				*this = internal::XMVectorToVector3(
					XMVectorScale(selfXM, scalar)
				);

				return *this;
			}

			Vector3& Vector3::operator/=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector3ToXMVector(Vector3(scalar, scalar, scalar));

				*this = internal::XMVectorToVector3(
					XMVectorDivide(selfXM, scalarXM)
				);

				return *this;
			}

			f32 Vector3::Length() const
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);

				return XMVectorGetX(XMVector3Length(selfXM));
			}

			f32 Vector3::LengthSq() const
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);

				return XMVectorGetX(XMVector3LengthSq(selfXM));
			}

			Vector3 Vector3::Cross(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return internal::XMVectorToVector3(
					XMVector3Cross(lhsXM, rhsXM)
				);
			}

			f32 Vector3::Dot(const Vector3& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector3ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector3ToXMVector(rhs);

				return XMVectorGetX(XMVector3Dot(lhsXM, rhsXM));
			}

			bool Vector3::IsNearlyZero() const
			{
				return IsApproxEqual(Vector3());
			}

			bool Vector3::IsApproxEqual(const Vector3& rhs) const
			{
				const bool xApproxEqual = math::ApproxEqual(x, rhs.x);
				const bool yApproxEqual = math::ApproxEqual(y, rhs.y);
				const bool zApproxEqual = math::ApproxEqual(z, rhs.z);
				
				return (xApproxEqual && yApproxEqual && zApproxEqual);
			}

			void Vector3::Normalize()
			{
				XMVECTOR selfXM = internal::Vector3ToXMVector(*this);

				*this = internal::XMVectorToVector3(
					XMVector3Normalize(selfXM)
				);
			}

			void Vector3::ToRadians()
			{
				x = math::ToRadians(x);
				y = math::ToRadians(y);
				z = math::ToRadians(z);
			}

			void Vector3::ToDegrees()
			{
				x = math::ToDegrees(x);
				y = math::ToDegrees(y);
				z = math::ToDegrees(z);
			}

			Vector3 Vector3::Reflect(const Vector3& incident, const Vector3& normal)
			{
				XMVECTOR incidentXM = internal::Vector3ToXMVector(
					incident
				);

				XMVECTOR normalXM = internal::Vector3ToXMVector(
					normal
				);

				return internal::XMVectorToVector3(
					XMVector3Reflect(incidentXM, normalXM)
				);
			}

			Vector3 Vector3::Up()
			{
				return Vector3(0.0f, 1.0f, 0.0f);
			}

			Vector3 Vector3::Right()
			{
				return Vector3(1.0f, 0.0f, 0.0f);
			}

			Vector3 Vector3::Down()
			{
				return Vector3(0.0f, -1.0f, 0.0f);
			}

			Vector3 Vector3::Left()
			{
				return Vector3(-1.0f, 0.0f, 0.0f);
			}

			Vector3 Vector3::Front()
			{
				return Vector3(0.0f, 0.0f, 1.0f);
			}

			Vector3 Vector3::Back()
			{
				return Vector3(0.0f, 0.0f, -1.0f);
			}

			Vector3 Vector3::Zero()
			{
				return Vector3();
			}

			Vector4::Vector4() :
				x(), y(), z(), w()
			{
			}

			Vector4::Vector4(const Vector3& v3, f32 w) :
				x(v3.x), y(v3.y), z(v3.z), w(w)
			{
			}

			Vector4::Vector4(f32 x, f32 y, f32 z, f32 w) :
				x(x), y(y), z(z), w(w)
			{
			}

			Vector4 Vector4::operator+() const
			{
				return *this;
			}

			Vector4 Vector4::operator-() const
			{
				return Vector4(-x, -y, -z, -w);
			}

			Vector4 Vector4::operator+(const Vector4& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector4ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector4ToXMVector(rhs);

				return internal::XMVectorToVector4(
					XMVectorAdd(lhsXM, rhsXM)
				);
			}

			Vector4 Vector4::operator-(const Vector4& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector4ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector4ToXMVector(rhs);

				return internal::XMVectorToVector4(
					XMVectorSubtract(lhsXM, rhsXM)
				);
			}

			Vector4 Vector4::operator*(const Vector4& rhs) const
			{
				XMVECTOR lhsXM = internal::Vector4ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector4ToXMVector(rhs);

				return internal::XMVectorToVector4(
					XMVectorMultiply(lhsXM, rhsXM)
				);
			}

			Vector4 Vector4::operator/(const Vector4& rhs) const
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector4ToXMVector(rhs);

				return internal::XMVectorToVector4(
					XMVectorDivide(selfXM, scalarXM)
				);
			}

			Vector4 Vector4::operator*(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);

				return internal::XMVectorToVector4(
					XMVectorScale(selfXM, scalar)
				);
			}

			Vector4 Vector4::operator/(f32 scalar) const
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector4ToXMVector(Vector4(scalar, scalar, scalar, scalar));

				return internal::XMVectorToVector4(
					XMVectorDivide(selfXM, scalarXM)
				);
			}

			Vector4& Vector4::operator+=(const Vector4& rhs)
			{
				XMVECTOR lhsXM = internal::Vector4ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector4ToXMVector(rhs);

				*this = internal::XMVectorToVector4(
					XMVectorAdd(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector4& Vector4::operator-=(const Vector4& rhs)
			{
				XMVECTOR lhsXM = internal::Vector4ToXMVector(*this);
				XMVECTOR rhsXM = internal::Vector4ToXMVector(rhs);

				*this = internal::XMVectorToVector4(
					XMVectorSubtract(lhsXM, rhsXM)
				);

				return *this;
			}

			Vector4& Vector4::operator*=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);

				*this = internal::XMVectorToVector4(
					XMVectorScale(selfXM, scalar)
				);

				return *this;
			}

			Vector4& Vector4::operator/=(f32 scalar)
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);
				XMVECTOR scalarXM = internal::Vector4ToXMVector(Vector4(scalar, scalar, scalar, scalar));

				*this = internal::XMVectorToVector4(
					XMVectorDivide(selfXM, scalarXM)
				);

				return *this;
			}

			f32 Vector4::Length() const
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);

				return XMVectorGetX(XMVector4Length(selfXM));
			}

			f32 Vector4::LengthSq() const
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);

				return XMVectorGetX(XMVector4LengthSq(selfXM));
			}

			bool Vector4::IsNearlyZero() const
			{
				return IsApproxEqual(Vector4());
			}

			bool Vector4::IsApproxEqual(const Vector4& rhs) const
			{
				const bool xApproxEqual = math::ApproxEqual(x, rhs.x);
				const bool yApproxEqual = math::ApproxEqual(y, rhs.y);
				const bool zApproxEqual = math::ApproxEqual(z, rhs.z);
				const bool wApproxEqual = math::ApproxEqual(w, rhs.w);

				return (xApproxEqual && yApproxEqual && zApproxEqual && wApproxEqual);
			}

			void Vector4::Normalize()
			{
				XMVECTOR selfXM = internal::Vector4ToXMVector(*this);

				*this = internal::XMVectorToVector4(
					XMVector4Normalize(selfXM)
				);
			}

			namespace internal
			{
				DirectX::XMVECTOR Vector2ToXMVector(const Vector2& vector)
				{
					XMFLOAT2 f2(vector.x, vector.y);

					return XMLoadFloat2(&f2);
				}

				Vector2 XMVectorToVector2(const DirectX::XMVECTOR& xmVector)
				{
					XMFLOAT2 f2 = {};
					XMStoreFloat2(&f2, xmVector);

					return Vector2(f2.x, f2.y);
				}

				DirectX::XMVECTOR Vector3ToXMVector(const Vector3& vector)
				{
					XMFLOAT3 f3(vector.x, vector.y, vector.z);

					return XMLoadFloat3(&f3);
				}

				Vector3 XMVectorToVector3(const DirectX::XMVECTOR& xmVector)
				{
					XMFLOAT3 f3 = {};
					XMStoreFloat3(&f3, xmVector);

					return Vector3(f3.x, f3.y, f3.z);
				}

				DirectX::XMVECTOR Vector4ToXMVector(const Vector4& vector)
				{
					XMFLOAT4 f4(vector.x, vector.y, vector.z, vector.w);

					return XMLoadFloat4(&f4);
				}

				Vector4 XMVectorToVector4(const DirectX::XMVECTOR& xmVector)
				{
					XMFLOAT4 f4 = {};
					XMStoreFloat4(&f4, xmVector);

					return Vector4(f4.x, f4.y, f4.z, f4.w);
				}
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

ts::kit::math::Vector2 operator*(ts::f32 scalar, const ts::kit::math::Vector2& rhs)
{
	return rhs * scalar;
}

ts::kit::math::Vector3 operator*(ts::f32 scalar, const ts::kit::math::Vector3& rhs)
{
	return rhs * scalar;
}

ts::kit::math::Vector4 operator*(ts::f32 scalar, const ts::kit::math::Vector4& rhs)
{
	return rhs * scalar;
}