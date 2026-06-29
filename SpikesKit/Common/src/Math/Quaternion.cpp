// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/Quaternion.hpp"
#include "Math/Matrix.hpp"

using namespace DirectX;

namespace ts
{
	namespace kit
	{
		namespace math
		{
			Quaternion::Quaternion() :
				x(0.0f), y(0.0f), z(0.0f), w(1.0f)
			{
			}

			Quaternion::Quaternion(f32 x, f32 y, f32 z, f32 w) :
				x(x), y(y), z(z), w(w)
			{
			}

			Quaternion Quaternion::operator*(const Quaternion& rhs) const
			{
				return Multiply(*this, rhs);
			}

			Quaternion& Quaternion::operator*=(const Quaternion& rhs)
			{
				*this = Multiply(*this, rhs);
				
				return *this;
			}

			Quaternion Quaternion::Conjugate() const
			{
				return internal::XMVectorToQuaternion(
					XMQuaternionConjugate(
						internal::QuaternionToXMVector(
							*this
						)
					)
				);
			}

			Quaternion Quaternion::Slerp(
				const Quaternion& rhs,
				f32 t
			) const
			{
				return internal::XMVectorToQuaternion(XMQuaternionSlerp(
					internal::QuaternionToXMVector(
						*this
					),
					internal::QuaternionToXMVector(
						rhs
					),
					t
				));
			}

			f32 Quaternion::Dot(const Quaternion& rhs) const
			{
				return XMVectorGetX(XMQuaternionDot(
					internal::QuaternionToXMVector(
						*this
					),
					internal::QuaternionToXMVector(
						rhs
					)
				));
			}

			Vector3 Quaternion::Rotate(const Vector3& v) const
			{
				return internal::XMVectorToVector3(XMVector3Rotate(
					internal::Vector3ToXMVector(v),
					internal::QuaternionToXMVector(*this)
				));

			}

			void Quaternion::Normalize()
			{
				*this = internal::XMVectorToQuaternion(XMQuaternionNormalize(
					internal::QuaternionToXMVector(
						*this
					)
				));
			}

			void Quaternion::ToMatrix(Matrix& outMatrix) const
			{
				outMatrix = internal::XMMatrixToMatrix(XMMatrixRotationQuaternion(
					internal::QuaternionToXMVector(
						*this
					)
				));
			}

			Quaternion Quaternion::Multiply(
				const Quaternion& lhs,
				const Quaternion& rhs
			)
			{
				return internal::XMVectorToQuaternion(
					XMQuaternionMultiply(
					internal::QuaternionToXMVector(lhs),
					internal::QuaternionToXMVector(rhs)
				));
			}

			Quaternion Quaternion::RotationRollPitchYaw(const Vector3& eulerAngles)
			{
				return internal::XMVectorToQuaternion(
					XMQuaternionRotationRollPitchYaw(
						eulerAngles.x,
						eulerAngles.y,
						eulerAngles.z
					)
				);
			}

			Quaternion Quaternion::RotationAxis(
				const Vector3& axis, 
				f32 radians
			)
			{
				return internal::XMVectorToQuaternion(
					XMQuaternionRotationAxis(
						internal::Vector3ToXMVector(axis),
						radians
					)
				);
			}

			namespace internal
			{
				DirectX::XMVECTOR QuaternionToXMVector(const Quaternion& q)
				{
					const XMFLOAT4 f4(q.x, q.y, q.z, q.w);

					return XMLoadFloat4(&f4);
				}

				Quaternion XMVectorToQuaternion(const DirectX::XMVECTOR& v)
				{
					XMFLOAT4 f4 = {};
					XMStoreFloat4(&f4, v);

					return Quaternion(f4.x, f4.y, f4.z, f4.w);
				}
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts