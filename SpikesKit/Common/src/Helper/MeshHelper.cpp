// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Helper/MeshHelper.hpp"

namespace ts
{
	namespace kit
	{
		namespace meshhelper
		{
			namespace internal
			{
				template <typename IndexType>
				[[nodiscard]]
				bool ComputeTangentsAndBitangentsImpl(
					const std::vector<Vector3>& positions,
					const std::vector<Vector3>& normals,
					const std::vector<Vector2>& uvs,
					const std::vector<IndexType>& indices,
					std::vector<Vector4>& outTangents
				)
				{
					static_assert(
						sizeof(IndexType) == sizeof(u8) ||
						sizeof(IndexType) == sizeof(u16) ||
						sizeof(IndexType) == sizeof(u32),
						"IndexTypeは u8/u16/u32でなければいけません"
					);

					std::vector<Vector3> sumFaceTangents(outTangents.size());
					std::vector<Vector3> sumFaceBitangents(outTangents.size());

					// 面単位でタンジェントとバイタンジェントの合計を求める
					for (u64 indicesIndex = 0u; indicesIndex < indices.size(); indicesIndex += 3u)
					{
						const IndexType index0 = indices[indicesIndex];
						const IndexType index1 = indices[indicesIndex + 1u];
						const IndexType index2 = indices[indicesIndex + 2u];

						const Vector3& v0 = positions[index0];
						const Vector3& v1 = positions[index1];
						const Vector3& v2 = positions[index2];

						const Vector2& uv0 = uvs[index0];
						const Vector2& uv1 = uvs[index1];
						const Vector2& uv2 = uvs[index2];

						// 辺の算出
						const Vector3 edge1 = v1 - v0;
						const Vector3 edge2 = v2 - v0;

						const Vector2 deltaUV1 = uv1 - uv0;
						const Vector2 deltaUV2 = uv2 - uv0;

						// 分母の算出(スケーリング値)
						const f32 determinant = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						// UVが潰れているなどエラー値がまぎれている
						if (std::abs(determinant) < 1.0e-6f)
						{
							TS_WARNING_LOG("UVが潰れているか無効な値が含まれています");

							outTangents.push_back(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

							continue;
						}

						// 接ベクトルの算出
						Vector3 tangent = {};
						tangent.x = determinant * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
						tangent.y = determinant * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
						tangent.z = determinant * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

						sumFaceTangents[index0] += tangent;
						sumFaceTangents[index1] += tangent;
						sumFaceTangents[index2] += tangent;

						// 従接ベクトルの算出
						Vector3 bitangent = {};
						bitangent.x = determinant * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x);
						bitangent.y = determinant * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y);
						bitangent.z = determinant * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z);

						sumFaceBitangents[index0] += bitangent;
						sumFaceBitangents[index1] += bitangent;
						sumFaceBitangents[index2] += bitangent;
					}

					// 頂点単位で計算
					for (u64 vertexIndex = 0; vertexIndex < positions.size(); ++vertexIndex)
					{
						const Vector3& normal = normals[vertexIndex];
						const Vector3& tangent = sumFaceTangents[vertexIndex];
						const Vector3& bitangent = sumFaceBitangents[vertexIndex];

						// グラム・シュミットの直交化を用いてNormalに対してTangentとBitangentを直角にする
						Vector3 adjustedTangent = (tangent) - (normal * normal.Dot(tangent));
						adjustedTangent.Normalize();

						const f32 sign = CalculateTangentSign(normal, tangent, bitangent);

						outTangents[vertexIndex] = Vector4(adjustedTangent, sign);
					}

					return true;
				}
			} // namespace internal

			[[nodiscard]]
			bool ComputeTangentsAndBitangents(
				const ComputeTangentsAndBitangentsParameters& parameters,
				std::vector<Vector4>& outTangents
			)
			{
				if (!parameters.m_indicesU8.empty())
				{
					return internal::ComputeTangentsAndBitangentsImpl<u8>(
						parameters.m_positions,
						parameters.m_normals,
						parameters.m_uvs,
						parameters.m_indicesU8,
						outTangents
					);
				}
				else if (!parameters.m_indicesU16.empty())
				{
					return internal::ComputeTangentsAndBitangentsImpl<u16>(
						parameters.m_positions,
						parameters.m_normals,
						parameters.m_uvs,
						parameters.m_indicesU16,
						outTangents
					);
				}
				else if (!parameters.m_indicesU32.empty())
				{
					return internal::ComputeTangentsAndBitangentsImpl<u32>(
						parameters.m_positions,
						parameters.m_normals,
						parameters.m_uvs,
						parameters.m_indicesU32,
						outTangents
					);
				}

				TS_ASSERT(false, "Indicesが設定されていません");

				return false;
			}

			f32 CalculateTangentSign(
				const Vector3& normal,
				const Vector3& tangent,
				const Vector3& bitangent
			)
			{
				Vector3 theoreticalBitangent = normal.Cross(tangent);
				theoreticalBitangent.Normalize();

				const f32 dot = theoreticalBitangent.Dot(bitangent);

				return (dot < 0.0f) ? -1.0f : 1.0f;
			}
		} // namespace meshhelper
	} // namespace kit
} // namespace ts