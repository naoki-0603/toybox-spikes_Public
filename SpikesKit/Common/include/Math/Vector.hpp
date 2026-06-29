// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_MATH_VECTOR_HPP_
#define SPIKES_KIT_COMMON_CORE_MATH_VECTOR_HPP_

namespace ts
{
	namespace kit
	{
		namespace math
		{
			struct Vector2
			{
				Vector2();
				Vector2(f32 x, f32 y);
				Vector2(const Vector2&) = default;
				Vector2(Vector2&&) noexcept = default;

				Vector2& operator=(const Vector2&) = default;
				Vector2& operator=(Vector2&&) noexcept = default;
				
				Vector2 operator+() const;
				Vector2 operator-() const;
				Vector2 operator+(const Vector2& rhs) const;
				Vector2 operator-(const Vector2& rhs) const;
				Vector2 operator*(const Vector2& rhs) const;
				Vector2 operator/(const Vector2& rhs) const;
				Vector2 operator*(f32 scalar) const;
				Vector2 operator/(f32 scalar) const;

				Vector2& operator+=(const Vector2& rhs);
				Vector2& operator-=(const Vector2& rhs);
				Vector2& operator*=(f32 scalar);
				Vector2& operator/=(f32 scalar);

				[[nodiscard]]
				f32 Length() const;

				[[nodiscard]]
				f32 LengthSq() const;

				[[nodiscard]]
				f32 Cross(const Vector2& rhs) const;

				[[nodiscard]]
				f32 Dot(const Vector2& rhs) const;

				[[nodiscard]]
				bool IsNearlyZero() const;

				[[nodiscard]]
				bool IsApproxEqual(const Vector2& rhs) const;

				void Normalize();

				f32 x, y;
			};

			struct Vector3
			{
				Vector3();
				Vector3(const Vector2& v2, f32 z);
				Vector3(f32 x, f32 y, f32 z);
				Vector3(const Vector3&) = default;
				Vector3(Vector3&&) noexcept = default;

				Vector3& operator=(const Vector3&) = default;
				Vector3& operator=(Vector3&&) noexcept = default;

				Vector3 operator+() const;
				Vector3 operator-() const;
				Vector3 operator+(const Vector3& rhs) const;
				Vector3 operator-(const Vector3& rhs) const;
				Vector3 operator*(const Vector3& rhs) const;
				Vector3 operator/(const Vector3& rhs) const;
				Vector3 operator*(f32 scalar) const;
				Vector3 operator/(f32 scalar) const;

				Vector3& operator+=(const Vector3& rhs);
				Vector3& operator-=(const Vector3& rhs);
				Vector3& operator*=(f32 scalar);
				Vector3& operator/=(f32 scalar);

				[[nodiscard]]
				f32 Length() const;

				[[nodiscard]]
				f32 LengthSq() const;

				[[nodiscard]]
				Vector3 Cross(const Vector3& rhs) const;

				[[nodiscard]]
				f32 Dot(const Vector3& rhs) const;

				[[nodiscard]]
				bool IsNearlyZero() const;

				[[nodiscard]]
				bool IsApproxEqual(const Vector3& rhs) const;

				void Normalize();

				void ToRadians();
				void ToDegrees();

				/**
				 * @param[in] incident 入射角
				 * @param[in] normal 法線
				 */
				[[nodiscard]]
				static Vector3 Reflect(const Vector3& incident, const Vector3& normal);

				[[nodiscard]]
				static Vector3 Up();

				[[nodiscard]]
				static Vector3 Right();

				[[nodiscard]]
				static Vector3 Down();

				[[nodiscard]]
				static Vector3 Left();

				[[nodiscard]]
				static Vector3 Front();

				[[nodiscard]]
				static Vector3 Back();

				[[nodiscard]]
				static Vector3 Zero();

				f32 x, y, z;
			};

			struct Vector4
			{
				Vector4();
				Vector4(const Vector3& v3, f32 w);
				Vector4(f32 x, f32 y, f32 z, f32 w);
				Vector4(const Vector4&) = default;
				Vector4(Vector4&&) noexcept = default;

				Vector4& operator=(const Vector4&) = default;
				Vector4& operator=(Vector4&&) noexcept = default;

				Vector4 operator+() const;
				Vector4 operator-() const;
				Vector4 operator+(const Vector4& rhs) const;
				Vector4 operator-(const Vector4& rhs) const;
				Vector4 operator*(const Vector4& rhs) const;
				Vector4 operator/(const Vector4& rhs) const;
				Vector4 operator*(f32 scalar) const;
				Vector4 operator/(f32 scalar) const;

				Vector4& operator+=(const Vector4& rhs);
				Vector4& operator-=(const Vector4& rhs);
				Vector4& operator*=(f32 scalar);
				Vector4& operator/=(f32 scalar);

				[[nodiscard]]
				f32 Length() const;

				[[nodiscard]]
				f32 LengthSq() const;

				[[nodiscard]]
				bool IsNearlyZero() const;

				[[nodiscard]]
				bool IsApproxEqual(const Vector4& rhs) const;

				void Normalize();

				f32 x, y, z, w;
			};

			namespace internal
			{
				[[nodiscard]]
				DirectX::XMVECTOR Vector2ToXMVector(const Vector2& vector);

				[[nodiscard]]
				Vector2 XMVectorToVector2(const DirectX::XMVECTOR& xmVector);

				[[nodiscard]]
				DirectX::XMVECTOR Vector3ToXMVector(const Vector3& vector);

				[[nodiscard]]
				Vector3 XMVectorToVector3(const DirectX::XMVECTOR& xmVector);

				[[nodiscard]]
				DirectX::XMVECTOR Vector4ToXMVector(const Vector4& vector);

				[[nodiscard]]
				Vector4 XMVectorToVector4(const DirectX::XMVECTOR& xmVector);
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

ts::kit::math::Vector2 operator*(ts::f32 scalar, const ts::kit::math::Vector2& rhs);
ts::kit::math::Vector2 operator/(ts::f32 scalar, const ts::kit::math::Vector2& rhs);

ts::kit::math::Vector3 operator*(ts::f32 scalar, const ts::kit::math::Vector3& rhs);
ts::kit::math::Vector3 operator/(ts::f32 scalar, const ts::kit::math::Vector3& rhs);

ts::kit::math::Vector4 operator*(ts::f32 scalar, const ts::kit::math::Vector4& rhs);
ts::kit::math::Vector4 operator/(ts::f32 scalar, const ts::kit::math::Vector4& rhs);

#endif // !SPIKES_KIT_COMMON_CORE_MATH_VECTOR_HPP_