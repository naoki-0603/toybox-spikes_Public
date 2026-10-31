// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/Debug/DebugRenderer.hpp"

#include "Graphics/RHI/RHI_Buffer.hpp"

#include "Graphics/RenderResourceManager.hpp"

#include "Helper/GraphicsHelper.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			DebugRenderer::DebugRenderer() :
				m_vertices(), m_vertexIndex(),
				m_vbHandle()
			{
			}

			bool DebugRenderer::Create(
				resource::RenderResourceManager* resourceManager
			)
			{
				// 頂点バッファの作成
				RHI_BufferDesc bufferDesc = {};
				bufferDesc.m_type = RHI_BufferType::VertexBuffer;
				bufferDesc.m_usage = RHI_BufferUsage::Dynamic;
				bufferDesc.m_strideInBytes = sizeof(DebugVertex);
				bufferDesc.m_count = internal::k_maxVertices;
				bufferDesc.m_data = nullptr;

				m_vbHandle = resourceManager->CreateBuffer(bufferDesc);
				TS_ASSERT(m_vbHandle.IsValid(), "頂点バッファの作成に失敗しました");
				
				m_vertices.resize(internal::k_maxVertices);

				return true;
			}

			bool DebugRenderer::Destroy()
			{
				return true;
			}

			void DebugRenderer::Begin()
			{
				m_vertexIndex = 0u;
			}

			void DebugRenderer::End(resource::RenderResourceManager* resourceManager)
			{
				// 頂点の書き込み
				RHI_Buffer* vb = resourceManager->FindBuffer(m_vbHandle);
				vb->WriteData(
					m_vertices.data(),
					m_vertices.size(),
					0u
				);
			}

			void DebugRenderer::DrawLine(const DebugLineParameters& parameters)
			{
				const u32 totalVertexCount = m_vertexIndex + 1u;
				TS_ASSERT(
					totalVertexCount < internal::k_maxVertices,
					"頂点数が最大数を超過しています"
				);

				const Vector3& begin = parameters.m_begin;
				const Vector4& beginColor = parameters.m_beginColor;
				const Vector3& end = parameters.m_end;
				const Vector4& endColor = parameters.m_endColor;

				// Begin
				{
					const u32 beginVertexIndex = m_vertexIndex;

					DebugVertex& vertex = m_vertices[beginVertexIndex];
					vertex.m_position = begin;
					vertex.m_color = graphicshelper::PackColor(
						beginColor
					);
				}

				// End
				{
					const u32 endVertexIndex = m_vertexIndex + 1u;

					DebugVertex& vertex = m_vertices[endVertexIndex];
					vertex.m_position = end;
					vertex.m_color = graphicshelper::PackColor(
						endColor
					);
				}

				m_vertexIndex += 2u;
			}

			void DebugRenderer::DrawCircle(const DebugCircleParameters& parameters)
			{
				const Vector3& center = parameters.m_center;
				const f32 radius = parameters.m_radius;
				const Quaternion& rotation = parameters.m_rotation;
				const Vector4& color = parameters.m_color;
				const u32 divisionCount = parameters.m_divisionCount;

				// 円を描く(X-Z平面)
				{
					for (u32 step = 0u; step < divisionCount; ++step)
					{
						const f32 beginRatio = static_cast<f32>(step) / static_cast<f32>(divisionCount);
						const f32 beginRadian = beginRatio * math::k_pi2;

						const f32 endRatio = static_cast<f32>(step + 1u) / static_cast<f32>(divisionCount);
						const f32 endRadian = endRatio * math::k_pi2;

						// ローカル基準で円を作成
						const Vector3 localBegin = { std::cosf(beginRadian) * radius, 0.0f, std::sinf(beginRadian) * radius };
						const Vector3 localEnd = { std::cosf(endRadian) * radius, 0.0f, std::sinf(endRadian) * radius };

						// 指定されたクオータニオンで回転させてから足し合わせる
						DebugLineParameters lineParameters = {};
						lineParameters.m_begin = center + rotation.Rotate(localBegin);
						lineParameters.m_beginColor = color;
						lineParameters.m_end = center + rotation.Rotate(localEnd);
						lineParameters.m_endColor = color;

						DrawLine(lineParameters);
					}
				}
			}

			void DebugRenderer::DrawHalfCircle(const DebugHalfCircleParameters& parameters)
			{
				const Vector3& center = parameters.m_center;
				const f32 radius = parameters.m_radius;
				const Quaternion& rotation = parameters.m_rotation;
				const Vector4& color = parameters.m_color;
				const u32 divisionCount = parameters.m_divisionCount;

				// 半円を描く(X-Y平面)
				{
					for (u32 step = 0u; step < divisionCount; ++step)
					{
						const f32 beginRatio = static_cast<f32>(step) / static_cast<f32>(divisionCount);
						const f32 beginRadian = beginRatio * math::k_pi;

						const f32 endRatio = static_cast<f32>(step + 1u) / static_cast<f32>(divisionCount);
						const f32 endRadian = endRatio * math::k_pi;

						// ローカル基準で円を作成
						const Vector3 localBegin = { std::cosf(beginRadian) * radius, std::sinf(beginRadian) * radius, 0.0f };
						const Vector3 localEnd = { std::cosf(endRadian) * radius, std::sinf(endRadian) * radius, 0.0f };

						// 指定されたクオータニオンで回転させてから足し合わせる
						DebugLineParameters lineParameters = {};
						lineParameters.m_begin = center + rotation.Rotate(localBegin);
						lineParameters.m_beginColor = color;
						lineParameters.m_end = center + rotation.Rotate(localEnd);
						lineParameters.m_endColor = color;

						DrawLine(lineParameters);
					}
				}
			}

			void DebugRenderer::DrawBox(const DebugBoxParameters& parameters)
			{
				const Vector3& max = parameters.m_max;
				const Vector3& min = parameters.m_min;
				const Vector4& color = parameters.m_color;

				const Vector3 fbl = { min.x, min.y, min.z }; // Front Bottom Left
				const Vector3 fbr = { max.x, min.y, min.z }; // Front Bottom Right
				const Vector3 bbl = { min.x, min.y, max.z }; // Back Bottom Left
				const Vector3 bbr = { max.x, min.y, max.z }; // Back Bottom Right

				const Vector3 ftl = { min.x, max.y, min.z }; // Front Top Left
				const Vector3 ftr = { max.x, max.y, min.z }; // Front Top Right
				const Vector3 btl = { min.x, max.y, max.z }; // Back Top Left
				const Vector3 btr = { max.x, max.y, max.z }; // Back Top Right

				// 下面 4本
				DrawLine({ fbl, fbr, color, color });
				DrawLine({ fbl, bbl, color, color });
				DrawLine({ bbl, bbr, color, color });
				DrawLine({ bbr, fbr, color, color });

				// 縦柱 4本
				DrawLine({ fbl, ftl, color, color });
				DrawLine({ fbr, ftr, color, color });
				DrawLine({ bbl, btl, color, color });
				DrawLine({ bbr, btr, color, color });

				// 上面 4本
				DrawLine({ ftl, ftr, color, color });
				DrawLine({ ftr, btr, color, color });
				DrawLine({ btr, btl, color, color });
				DrawLine({ btl, ftl, color, color });
			}

			void DebugRenderer::DrawSphere(const DebugSphereParameters& parameters)
			{
				// 8分割で様子見
				constexpr u32 k_divisionCount = 8u;

				const Vector3& center = parameters.m_center;
				const f32 radius = parameters.m_radius;
				const Vector4& color = parameters.m_color;

				const Quaternion rotationXZ = { 0.0f, 0.0f, 0.0f, 1.0f };
				const Quaternion rotationXY = Quaternion::RotationAxis(Vector3::Right(), math::k_pidiv2);
				const Quaternion rotationZY = Quaternion::RotationAxis(Vector3::Front(), math::k_pidiv2);

				DrawCircle({ center, radius, rotationXZ, color, k_divisionCount }); // XZ
				DrawCircle({ center, radius, rotationXY, color, k_divisionCount }); // XY
				DrawCircle({ center, radius, rotationZY, color, k_divisionCount }); // ZY
			}

			void DebugRenderer::DrawCone(const DebugConeParameters& parameters)
			{
				constexpr u32 k_divisionCount = 8u;

				const Vector3& apex = parameters.m_apex;
				const f32 radius = parameters.m_radius;
				const f32 height = parameters.m_height;
				const Quaternion& rotation = parameters.m_rotation;
				const Vector4& color = parameters.m_color;

				// 底面を求める
				const Vector3 bottomCenter = apex + rotation.Rotate({ 0.0f, height, 0.0f });

				// 中心軸の描画
				DrawLine({ apex, bottomCenter, color, color });

				// 底面の面を描画
				DrawCircle({ bottomCenter, radius, rotation, color });

				// 頂点から底面の円周4方向(0度, 90度, 180度, 270度)へ母線を引く
				const Vector3 localRimPoints[4] =
				{
					{  radius, height, 0.0f  },
					{ -radius, height, 0.0f  },
					{  0.0f, height,  radius },
					{  0.0f, height, -radius }
				};

				for (const auto& localPoint : localRimPoints)
				{
					const Vector3 worldPoint = apex + rotation.Rotate(localPoint);

					DrawLine({ apex, worldPoint, color, color });
				}
			}

			void DebugRenderer::DrawCapsule(const DebugCapsuleParameters& parameters)
			{
				constexpr u32 k_divisionCount = 8u;
				constexpr u32 k_halfDivisionCount = k_divisionCount / 2u;

				const Vector3& center = parameters.m_center;
				const f32 radius = parameters.m_radius;
				const f32 halfHeight = parameters.m_cylinderHeight * 0.5f;
				const Quaternion& rotation = parameters.m_rotation;
				const Vector4& color = parameters.m_color;

				// 上面、下面の球の中心座標を求める
				const Vector3 topCenter = center + rotation.Rotate({ 0.0f, halfHeight, 0.0f });
				const Vector3 bottomCenter = center + rotation.Rotate({ 0.0f, -halfHeight, 0.0f });

				// シリンダーの縦線4本を描画
				{
					const Vector3 sideOffsets[4] =
					{
						{ radius, 0.0f, 0.0f },
						{ -radius, 0.0f, 0.0f },
						{ 0.0f, 0.0f,  radius },
						{ 0.0f, 0.0f, -radius }
					};

					for (const auto& offset : sideOffsets)
					{
						const Vector3 begin = center + rotation.Rotate({ offset.x, -halfHeight, offset.z });
						const Vector3 end = center + rotation.Rotate({ offset.x, halfHeight, offset.z });

						DrawLine({ begin, end, color, color });
					}
				}
				
				// 上下の水平リング (XZ平面)
				DrawCircle({ topCenter, radius, rotation, color, k_divisionCount });
				DrawCircle({ bottomCenter, radius, rotation, color, k_divisionCount });

				// 上下のドーム半円 (XY,ZY平面)
				const Quaternion rotationY90 = Quaternion::RotationAxis(
					Vector3::Up(), math::k_pidiv2
				);

				const Quaternion rotationZ180 = Quaternion::RotationAxis(
					Vector3::Front(), math::k_pi
				);

				// 上面
				DrawHalfCircle({ topCenter, radius, rotation, color, k_halfDivisionCount }); // XY
				DrawHalfCircle({ topCenter, radius, rotationY90 * rotation, color, k_halfDivisionCount }); // ZY

				// 下面
				
				// rotationZ180 をかけてY- に変換
				DrawHalfCircle({ bottomCenter, radius, rotationZ180 * rotation, color, k_halfDivisionCount }); // XY
				DrawHalfCircle({ bottomCenter, radius, rotationY90 * rotationZ180 * rotation, color, k_halfDivisionCount }); // ZY
			}

			void DebugRenderer::DrawArrow(const DebugArrowParameters& parameters)
			{
				const Vector3& start = parameters.m_start;
				const Vector3& direction = parameters.m_direction;
				const f32 length = parameters.m_length;
				const f32 headSize = parameters.m_headSize;
				const Vector4& color = parameters.m_color;

				// 横軸の描画
				const Vector3 end = start + (direction * length);

				DrawLine({ start, end, color, color });

				// directionに対して垂直な2軸を求める
				const Vector3 refUp = (direction.y < 0.99f) ? Vector3::Up() : Vector3::Right();

				Vector3 right = direction.Cross(refUp);
				right.Normalize();

				Vector3 up = direction.Cross(right);
				up.Normalize();

				// ヘッドから羽を引く
				const Vector3 headBase = end - (direction * headSize);
				const f32 headSpread = headSize * 0.5f;

				DrawLine({ end, headBase + (right * headSpread), color, color });
				DrawLine({ end, headBase - (right * headSpread), color, color });
				DrawLine({ end, headBase + (up * headSpread), color, color });
				DrawLine({ end, headBase - (up * headSpread), color, color });
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts
