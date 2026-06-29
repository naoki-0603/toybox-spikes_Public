// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_MATH_QUATERNION_HPP_
#define SPIKES_KIT_COMMON_CORE_MATH_QUATERNION_HPP_

#include "Vector.hpp"

namespace ts
{
	namespace kit
	{
		namespace math
		{
			struct Matrix;

			struct Quaternion
			{
				/// 単位四元数で初期化する(0, 0, 0, 1)
				Quaternion();
				Quaternion(f32 x, f32 y, f32 z, f32 w);
				Quaternion(const Quaternion&) = default;
				Quaternion(Quaternion&&) noexcept = default;

				Quaternion& operator=(const Quaternion&) = default;
				Quaternion& operator=(Quaternion&&) noexcept = default;

				Quaternion operator*(const Quaternion& rhs) const;
				Quaternion& operator*=(const Quaternion& rhs);

				[[nodiscard]]
				Quaternion Conjugate() const;

				/*
				* @param[in] t 補完係数 0.0f ~ 1.0f 
				*/
				[[nodiscard]]
				Quaternion Slerp(const Quaternion& rhs, f32 t) const;

				[[nodiscard]]
				f32 Dot(const Quaternion& rhs) const;

				[[nodiscard]]
				Vector3 Rotate(const Vector3& v) const;

				void Normalize();

				void ToMatrix(Matrix& outMatrix) const;

				[[nodiscard]]
				static Quaternion Multiply(
					const Quaternion& lhs,
					const Quaternion& rhs
				);

				// eulerAngles.x = pitch (X)
				// eulerAngles.y = yaw   (Y)
				// eulerAngles.z = roll  (Z)
				// Roll(Z) -> Pitch(X) -> Yaw(Y)の形で変換
				// Unit: radians
				[[nodiscard]]
				static Quaternion RotationRollPitchYaw(
					const Vector3& eulerAngles
				);

				[[nodiscard]]
				static Quaternion RotationAxis(
					const Vector3& axis,
					f32 radians
				);

				f32 x, y, z, w;
			};

			namespace internal
			{
				[[nodiscard]]
				DirectX::XMVECTOR QuaternionToXMVector(const Quaternion& q);

				[[nodiscard]]
				Quaternion XMVectorToQuaternion(const DirectX::XMVECTOR& v);
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_MATH_QUATERNION_HPP_