// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_MATH_MATRIX_HPP_
#define SPIKES_KIT_COMMON_CORE_MATH_MATRIX_HPP_

namespace ts
{
	namespace kit
	{
		namespace math
		{
			struct Quaternion;

			/**
			 * @brief 4x4の行列。
			 * @details 行優先で左手座標軸。（DirectX基準）
			 */
			struct Matrix
			{
				/**
				 * @brief 単位行列として初期化する
				 */
				Matrix();
				
				Matrix(
					f32 _11, f32 _12, f32 _13, f32 _14,
					f32 _21, f32 _22, f32 _23, f32 _24,
					f32 _31, f32 _32, f32 _33, f32 _34,
					f32 _41, f32 _42, f32 _43, f32 _44
				);

				/// mは16要素でなければならない
				Matrix(
					const f32* m
				);

				Matrix(
					const f32 (&m)[4][4]
				);
				
				Matrix(
					const Vector3& right,
					const Vector3& up,
					const Vector3& front
				);
				
				Matrix(
					const Vector3& right,
					const Vector3& up,
					const Vector3& front,
					const Vector3& offset
				);

				Matrix(
					const Vector4& right,
					const Vector4& up,
					const Vector4& front
				);

				Matrix(
					const Vector4& right,
					const Vector4& up,
					const Vector4& front,
					const Vector4& offset
				);

				Matrix(const Matrix&) = default;
				Matrix(Matrix&&) noexcept = default;
				~Matrix() noexcept = default;

				Matrix& operator=(const Matrix&) = default;
				Matrix& operator=(Matrix&&) noexcept = default;

				Matrix operator*(const Matrix& rhs) const;
				Matrix& operator*=(const Matrix& rhs);

				[[nodiscard]]
				Vector3 TransformCoord(const Vector3& v) const;

				[[nodiscard]]
				Vector3 TransformNormal(const Vector3& v) const;

				[[nodiscard]]
				Matrix Inverse() const;

				[[nodiscard]]
				Matrix Transpose() const;

				void SetRightV3(const Vector3& right);
				void SetUpV3(const Vector3& up);
				void SetFrontV3(const Vector3& front);
				void SetOffsetV3(const Vector3& offset);

				void SetRightV4(const Vector4& right);
				void SetUpV4(const Vector4& up);
				void SetFrontV4(const Vector4& front);
				void SetOffsetV4(const Vector4& offset);
				
				void Compose(
					const Vector3& scale,
					const Vector3& eulerAngles,
					const Vector3& offset
				);

				void Decompose(
					Vector3& outScale,
					Vector3& outEulerAngles,
					Vector3& outOffset
				) const;

				void ToQuaternion(
					Quaternion& outQuaternion
				) const;

				[[nodiscard]]
				Vector3 GetRightV3() const;

				[[nodiscard]]
				Vector3 GetUpV3() const;

				[[nodiscard]]
				Vector3 GetFrontV3() const;

				[[nodiscard]]
				Vector3 GetOffsetV3() const;

				[[nodiscard]]
				Vector4 GetRightV4() const;

				[[nodiscard]]
				Vector4 GetUpV4() const;

				[[nodiscard]]
				Vector4 GetFrontV4() const;

				[[nodiscard]]
				Vector4 GetOffsetV4() const;

				[[nodiscard]]
				std::array<f32, 16u> GetArray() const;

				[[nodiscard]]
				Vector3 ObtainEulerAngles() const;

				[[nodiscard]]
				Vector3 ObtainScaling() const;

				[[nodiscard]]
				static Matrix Multiply(const Matrix& m1, const Matrix& m2);

				[[nodiscard]]
				static Matrix Translation(const Vector3& offset);

				[[nodiscard]]
				static Matrix Scaling(const Vector3& scale);

				// eulerAngles.x = pitch (X)
				// eulerAngles.y = yaw   (Y)
				// eulerAngles.z = roll  (Z)
				// Roll(Z) -> Pitch(X) -> Yaw(Y)の形で変換
				// Unit: radians
				[[nodiscard]]
				static Matrix RotationRollPitchYaw(const Vector3& eulerAngles);

				[[nodiscard]]
				static Matrix RotationAxis(f32 radians, const Vector3& axis);

				[[nodiscard]]
				static Matrix LookAtLH(
					const Vector3& eye,
					const Vector3& focus,
					const Vector3& up = Vector3::Up()
				);

				[[nodiscard]]
				static Matrix PerspectiveFovLH(
					f32 fovAngleY,
					f32 aspectRatio,
					f32 nearZ,
					f32 farZ
				);

				[[nodiscard]]
				static Matrix OrthographicLH(
					f32 viewWidth,
					f32 viewHeight,
					f32 nearZ,
					f32 farZ
				);

				f32 _11, _12, _13, _14;
				f32 _21, _22, _23, _24;
				f32 _31, _32, _33, _34;
				f32 _41, _42, _43, _44;
			};

			namespace internal
			{
				[[nodiscard]]
				DirectX::XMMATRIX MatrixToXMMatrix(const Matrix& m);

				[[nodiscard]]
				Matrix XMMatrixToMatrix(const DirectX::XMMATRIX& xm);
			} // namespace internal
		} // namespace math
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_MATH_MATRIX_HPP_