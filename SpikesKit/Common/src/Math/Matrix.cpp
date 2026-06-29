// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"

#include "Math/Math.hpp"

using namespace DirectX;

namespace ts
{
	namespace kit
	{
		namespace math
		{
			Matrix::Matrix() :
				_11(1.0f), _12(), _13(), _14(),
				_21(), _22(1.0f), _23(), _24(),
				_31(), _32(), _33(1.0f), _34(),
				_41(), _42(), _43(), _44(1.0f)
			{
			}

			Matrix::Matrix(
				f32 _11, f32 _12, f32 _13, f32 _14,
				f32 _21, f32 _22, f32 _23, f32 _24,
				f32 _31, f32 _32, f32 _33, f32 _34,
				f32 _41, f32 _42, f32 _43, f32 _44
			) : _11(_11), _12(_12), _13(_13), _14(_14),
				_21(_21), _22(_22), _23(_23), _24(_24),
				_31(_31), _32(_32), _33(_33), _34(_34),
				_41(_41), _42(_42), _43(_43), _44(_44)
			{
			}

			Matrix::Matrix(const f32* m) :
				_11(m[0]), _12(m[1]), _13(m[2]), _14(m[3]),
				_21(m[4]), _22(m[5]), _23(m[6]), _24(m[7]),
				_31(m[8]), _32(m[9]), _33(m[10]), _34(m[11]),
				_41(m[12]), _42(m[13]), _43(m[14]), _44(m[15])
			{
			}

			Matrix::Matrix(const f32(&m)[4][4]) :
				_11(m[0][0]), _12(m[0][1]), _13(m[0][2]), _14(m[0][3]),
				_21(m[1][0]), _22(m[1][1]), _23(m[1][2]), _24(m[1][3]),
				_31(m[2][0]), _32(m[2][1]), _33(m[2][2]), _34(m[2][3]),
				_41(m[3][0]), _42(m[3][1]), _43(m[3][2]), _44(m[3][3])
			{
			}

			Matrix::Matrix(
				const Vector3& right,
				const Vector3& up,
				const Vector3& front
			) : _11(right.x), _12(right.y), _13(right.z), _14(),
				_21(up.x), _22(up.y), _23(up.z), _24(),
				_31(front.x), _32(front.y), _33(front.z), _34(),
				_41(), _42(), _43(), _44(1.0f)
			{
			}

			Matrix::Matrix(
				const Vector3& right,
				const Vector3& up,
				const Vector3& front,
				const Vector3& offset
			) : _11(right.x), _12(right.y), _13(right.z), _14(),
				_21(up.x), _22(up.y), _23(up.z), _24(),
				_31(front.x), _32(front.y), _33(front.z), _34(),
				_41(offset.x), _42(offset.y), _43(offset.z), _44(1.0f)
			{
			}

			Matrix::Matrix(
				const Vector4& right,
				const Vector4& up,
				const Vector4& front
			) : _11(right.x), _12(right.y), _13(right.z), _14(right.w),
				_21(up.x), _22(up.y), _23(up.z), _24(up.w),
				_31(front.x), _32(front.y), _33(front.z), _34(front.w),
				_41(), _42(), _43(), _44(1.0f)
			{
			}

			Matrix::Matrix(
				const Vector4& right,
				const Vector4& up,
				const Vector4& front,
				const Vector4& offset
			) : _11(right.x), _12(right.y), _13(right.z), _14(right.w),
				_21(up.x), _22(up.y), _23(up.z), _24(up.w),
				_31(front.x), _32(front.y), _33(front.z), _34(front.w),
				_41(offset.x), _42(offset.y), _43(offset.z), _44(offset.w)
			{
			}

			Matrix Matrix::operator*(const Matrix& rhs) const
			{
				return Multiply(*this, rhs);
			}

			Matrix& Matrix::operator*=(const Matrix& rhs)
			{
				*this = Multiply(*this, rhs);
			
				return *this;
			}

			Vector3 Matrix::TransformCoord(const Vector3& v) const
			{
				return internal::XMVectorToVector3(
					XMVector3TransformCoord(
					internal::Vector3ToXMVector(v),
					internal::MatrixToXMMatrix(*this)
				));
			}

			Vector3 Matrix::TransformNormal(const Vector3& v) const
			{
				return internal::XMVectorToVector3(
					XMVector3TransformNormal(
						internal::Vector3ToXMVector(v),
						internal::MatrixToXMMatrix(*this)
					));
			}

			Matrix Matrix::Inverse() const
			{
				return internal::XMMatrixToMatrix(
					XMMatrixInverse(
						nullptr, 
						internal::MatrixToXMMatrix(
							*this
						)
					)
				);
			}

			Matrix Matrix::Transpose() const
			{
				return internal::XMMatrixToMatrix(
					XMMatrixTranspose(
						internal::MatrixToXMMatrix(
							*this
						)
					)
				);
			}

			void Matrix::SetRightV3(const Vector3& right)
			{
				_11 = right.x;
				_12 = right.y;
				_13 = right.z;
			}

			void Matrix::SetUpV3(const Vector3& up)
			{
				_21 = up.x;
				_22 = up.y;
				_23 = up.z;
			}

			void Matrix::SetFrontV3(const Vector3& front)
			{
				_31 = front.x;
				_32 = front.y;
				_33 = front.z;
			}

			void Matrix::SetOffsetV3(const Vector3& offset)
			{
				_41 = offset.x;
				_42 = offset.y;
				_43 = offset.z;
			}

			void Matrix::SetRightV4(const Vector4& right)
			{
				SetRightV3(Vector3(right.x, right.y, right.z));

				_14 = right.w;
			}

			void Matrix::SetUpV4(const Vector4& up)
			{
				SetUpV3(Vector3(up.x, up.y, up.z));

				_24 = up.w;
			}

			void Matrix::SetFrontV4(const Vector4& front)
			{
				SetFrontV3(Vector3(front.x, front.y, front.z));

				_34 = front.w;
			}

			void Matrix::SetOffsetV4(const Vector4& offset)
			{
				SetOffsetV3(Vector3(offset.x, offset.y, offset.z));

				_44 = offset.w;
			}

			void Matrix::Compose(
				const Vector3& scale,
				const Vector3& eulerAngles,
				const Vector3& offset
			)
			{
				*this = Scaling(scale) * 
						RotationRollPitchYaw(eulerAngles) * 
						Translation(offset);
			}

			void Matrix::Decompose(
				Vector3& outScale,
				Vector3& outEulerAngles,
				Vector3& outOffset
			) const
			{
				outScale = ObtainScaling();
				outEulerAngles = ObtainEulerAngles();
				outOffset = GetOffsetV3();
			}

			void Matrix::ToQuaternion(
				Quaternion& outQuaternion
			) const
			{
				outQuaternion = internal::XMVectorToQuaternion(
					XMQuaternionRotationMatrix(
						internal::MatrixToXMMatrix(
							*this
						)
					)
				);
			}

			Vector3 Matrix::GetRightV3() const
			{
				return Vector3(_11, _12, _13);
			}

			Vector3 Matrix::GetUpV3() const
			{
				return Vector3(_21, _22, _23);
			}

			Vector3 Matrix::GetFrontV3() const
			{
				return Vector3(_31, _32, _33);
			}

			Vector3 Matrix::GetOffsetV3() const
			{
				return Vector3(_41, _42, _43);
			}

			Vector4 Matrix::GetRightV4() const
			{
				return Vector4(GetRightV3(), _14);
			}

			Vector4 Matrix::GetUpV4() const
			{
				return Vector4(GetUpV3(), _24);
			}

			Vector4 Matrix::GetFrontV4() const
			{
				return Vector4(GetFrontV3(), _34);
			}

			Vector4 Matrix::GetOffsetV4() const
			{
				return Vector4(GetOffsetV3(), _44);
			}

			std::array<f32, 16u> Matrix::GetArray() const
			{
				return
				{
					_11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44
				};
			}

			Vector3 Matrix::ObtainEulerAngles() const
			{
				// https://www.geometrictools.com/Documentation/EulerAngles.pdf

				// 11, 12, 13, 14 (r00, r01, r02, r03)
				// 21, 22, 23, 24 (r10, r11, r12, r13)
				// 31, 32, 33, 34 (r20, r21, r22, r23)
				// 41, 42, 43, 44 (r30, r31, r32, r33)

				const f32 r21 = _32;

				f32 thetaX = 0.0f;
				f32 thetaZ = 0.0f;
				f32 thetaY = 0.0f;

				if (r21 < +1.0f)
				{
					if (r21 > -1.0f)
					{
						thetaX = std::asinf(r21);
						thetaZ = std::atan2f(-_12, _22);
						thetaY = std::atan2f(-_31, _33);
					}
					else
					{
						
						thetaX = -k_pidiv2;
						thetaZ = -std::atan2f(_13, _11);
						thetaY = 0.0f;
					}
				}
				else
				{
					thetaX = k_pidiv2;
					thetaZ = std::atan2f(_13, _11);
					thetaY = 0.0f;
				}

				return Vector3(thetaX, thetaY, thetaZ);
			}

			Vector3 Matrix::ObtainScaling() const
			{
				// http://marupeke296.sakura.ne.jp/DXG_No39_WorldMatrixInformation.html
				const f32 x = XMVectorGetX(XMVector3Length(XMVectorSet(_11, _12, _13, 0.0f)));
				const f32 y = XMVectorGetX(XMVector3Length(XMVectorSet(_21, _22, _23, 0.0f)));
				const f32 z = XMVectorGetX(XMVector3Length(XMVectorSet(_31, _32, _33, 0.0f)));

				return Vector3(x, y, z);
			}

			Matrix Matrix::Multiply(const Matrix& m1, const Matrix& m2)
			{
				XMMATRIX m1XM = internal::MatrixToXMMatrix(m1);
				XMMATRIX m2XM = internal::MatrixToXMMatrix(m2);

				return internal::XMMatrixToMatrix(
					XMMatrixMultiply(
						m1XM, m2XM
					)
				);
			}

			Matrix Matrix::Translation(const Vector3& offset)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixTranslation(
						offset.x,
						offset.y,
						offset.z
					)
				);
			}

			Matrix Matrix::Scaling(const Vector3& scale)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixScaling(
						scale.x,
						scale.y,
						scale.z
					)
				);
			}

			Matrix Matrix::RotationRollPitchYaw(const Vector3& eulerAngles)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixRotationRollPitchYaw(
						eulerAngles.x,
						eulerAngles.y,
						eulerAngles.z
					)
				);
			}

			Matrix Matrix::RotationAxis(f32 radians, const Vector3& axis)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixRotationAxis(
						internal::Vector3ToXMVector(
							axis
						),
						radians
					)
				);
			}

			Matrix Matrix::LookAtLH(
				const Vector3& eye,
				const Vector3& focus,
				const Vector3& up
			)
			{
				XMVECTOR eyeXM = internal::Vector3ToXMVector(eye);
				XMVECTOR focusXM = internal::Vector3ToXMVector(focus);
				XMVECTOR upXM = internal::Vector3ToXMVector(up);

				return internal::XMMatrixToMatrix(
					XMMatrixLookAtLH(
						eyeXM,
						focusXM,
						upXM
					)
				);
			}

			Matrix Matrix::PerspectiveFovLH(
				f32 fovAngleY,
				f32 aspectRatio,
				f32 nearZ,
				f32 farZ
			)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixPerspectiveFovLH(
						fovAngleY,
						aspectRatio,
						nearZ,
						farZ
					)
				);
			}

			Matrix Matrix::OrthographicLH(
				f32 viewWidth,
				f32 viewHeight,
				f32 nearZ,
				f32 farZ
			)
			{
				return internal::XMMatrixToMatrix(
					XMMatrixOrthographicLH(
						viewWidth,
						viewHeight,
						nearZ,
						farZ
					)
				);
			}

			namespace internal
			{
				[[nodiscard]]
				DirectX::XMMATRIX MatrixToXMMatrix(const Matrix& m)
				{
					XMFLOAT4X4 f4x4(m.GetArray().data());

					return XMLoadFloat4x4(&f4x4);
				}

				[[nodiscard]]
				Matrix XMMatrixToMatrix(const DirectX::XMMATRIX& xm)
				{
					XMFLOAT4X4 f4x4 = {};
					XMStoreFloat4x4(&f4x4, xm);

					return Matrix(f4x4.m);
				}
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts
