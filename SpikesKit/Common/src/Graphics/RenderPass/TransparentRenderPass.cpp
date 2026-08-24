// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RenderPass/TransparentRenderPass.hpp"

#include "Graphics/RHI/RHI_Device.hpp"
#include "Graphics/RHI/RHI_PipelineState.hpp"
#include "Graphics/RHI/RHI_Shader.hpp"
#include "Graphics/RHI/RHI_ShaderCompiler.hpp"
#include "Graphics/RHI/RHI_ShaderBindings.hpp"

#include "Graphics/RenderResourceManager.hpp"

#include "Graphics/RenderCommand/RenderCommandRecorder.hpp"
#include "Graphics/RenderCommand/RenderResourceRecorder.hpp"

#include "Graphics/ObjectConstants.hpp"

#include "Core/Thread/RenderJobSystem.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			static void ExecuteTransparentRenderPassJob1(void* data)
			{
				ZoneScopedNC("TransparentRenderPassJob1", tracy::Color::Orange);

				// パスデータの取得
				auto passData = reinterpret_cast<const TransparentRenderPassData*>(data);

				// 必要なリソースデータの取得
				const std::vector<RenderPacket>& renderPackets = *passData->m_renderPackets;
				const resource::RenderResourceManager* resourceManager = passData->m_renderResourceManager;
				RenderResourceRecorder* resourceRecorder = passData->m_resourceRecorder;
				RenderCommandRecorder* commandRecorder = passData->m_recorder;

				// シーン定数バッファを設定する
				const RHI_Buffer* sceneConstantsBuffer = resourceManager->FindBuffer(
					passData->m_sceneConstantsBufferHandle
				);

				// 半透明用のパイプラインステートを追加
				{
					const RHI_PipelineState* pipelineState = resourceManager->FindPipelineState(
						passData->m_pipelineStateHandle
					);

					commandRecorder->SetPipelineState(
						pipelineState
					);
				}

				// パケットをループさせる
				for (u64 packetIndex = 0u; packetIndex < renderPackets.size(); ++packetIndex)
				{
					const RenderPacket& renderPacket = renderPackets[packetIndex];

					// シェーダーバインディング
					const auto shaderBindings = resourceRecorder->AllocateObject<RHI_ShaderBindings>();

					// シーンの定数バッファ
					{
						shaderBindings->SetConstantBuffer(
							0u,
							sceneConstantsBuffer
						);
					}

					// 頂点バッファ
					{
						const RHI_Buffer* vertexBuffer = resourceManager->FindBuffer(
							renderPacket.m_vertexBufferHandle
						);

						const auto vertexBufferArray = resourceRecorder->AllocateArray<const RHI_Buffer*>();
						vertexBufferArray[0] = vertexBuffer;

						commandRecorder->SetVertexBuffers(
							0u,
							1u,
							vertexBufferArray
						);
					}

					// インデックスバッファ
					{
						const RHI_Buffer* indexBuffer = resourceManager->FindBuffer(
							renderPacket.m_indexBufferHandle
						);

						RHI_Format indexBufferFormat = RHI_Format::Unknown;
						switch (indexBuffer->GetStride())
						{
						case 1: // u8
							indexBufferFormat = RHI_Format::R8_UInt;
							break;
						case 2: // u16
							indexBufferFormat = RHI_Format::R16_UInt;
							break;
						case 4: // u32
							indexBufferFormat = RHI_Format::R32_UInt;
							break;
						}

						commandRecorder->SetIndexBuffer(
							indexBufferFormat,
							indexBuffer
						);
					}

					// シェーダーバインディング
					{
						// マテリアル
						{
							// 定数バッファ
							{
								const RHI_Buffer* materialConstantBuffer = resourceManager->FindBuffer(
									renderPacket.m_materialConstantBufferHandle
								);

								shaderBindings->SetConstantBuffer(
									2u,
									materialConstantBuffer
								);
							}

							// ベースカラー
							if (renderPacket.m_baseColorTextureHandle.IsValid())
							{
								const RHI_TextureView* baseColorTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_baseColorTextureHandle
								);

								shaderBindings->SetTextureView(
									0u,
									baseColorTextureView
								);
							}

							// メタリックラフネス
							if (renderPacket.m_metallicRoughnessTextureHandle.IsValid())
							{
								const RHI_TextureView* metallicRoughnessTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_metallicRoughnessTextureHandle
								);

								shaderBindings->SetTextureView(
									1u,
									metallicRoughnessTextureView
								);
							}

							// ノーマル
							if (renderPacket.m_normalTextureHandle.IsValid())
							{
								const RHI_TextureView* normalTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_normalTextureHandle
								);

								shaderBindings->SetTextureView(
									2u,
									normalTextureView
								);
							}

							// オクルージョン
							if (renderPacket.m_occlusionTextureHandle.IsValid())
							{
								const RHI_TextureView* occlusionTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_occlusionTextureHandle
								);

								shaderBindings->SetTextureView(
									3u,
									occlusionTextureView
								);
							}

							// エミッシブ
							if (renderPacket.m_emissiveTextureHandle.IsValid())
							{
								const RHI_TextureView* emissiveTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_emissiveTextureHandle
								);

								shaderBindings->SetTextureView(
									4u,
									emissiveTextureView
								);
							}
						}
					}

					// オブジェクト定数バッファ
					{
						// オブジェクトの定数バッファを更新する
						const RHI_Buffer* objectConstantBuffer = resourceManager->FindBuffer(
							passData->m_objectConstantsBufferHandle
						);

						auto objectConstants = resourceRecorder->AllocateArray<ObjectConstants>();
						objectConstants->m_worldTransform = renderPacket.m_worldTransform;

						commandRecorder->UpdateConstantBuffer(
							objectConstants,
							sizeof(ObjectConstants),
							objectConstantBuffer
						);

						// バインド
						shaderBindings->SetConstantBuffer(
							1u,
							objectConstantBuffer
						);
					}

					// シェーダーバインディングを設定
					{
						commandRecorder->SetShaderBindings(
							shaderBindings
						);
					}

					// 描画コマンド
					{
						commandRecorder->DrawIndexed(
							renderPacket.m_indexCount,
							renderPacket.m_startIndexLocation,
							0u
						);
					}
				}
			}

			static void ExecuteTransparentRenderPassJob2(void* data)
			{
				ZoneScopedNC("TransparentRenderPassJob2", tracy::Color::Orange);

				// パスデータの取得
				auto passData = reinterpret_cast<const TransparentRenderPassData*>(data);

				// 必要なリソースデータの取得
				const std::vector<RenderPacket>& renderPackets = *passData->m_renderPackets;
				const resource::RenderResourceManager* resourceManager = passData->m_renderResourceManager;
				RenderResourceRecorder* resourceRecorder = passData->m_resourceRecorder;
				RenderCommandRecorder* commandRecorder = passData->m_recorder;

				// シーン定数バッファを設定する
				const RHI_Buffer* sceneConstantsBuffer = resourceManager->FindBuffer(
					passData->m_sceneConstantsBufferHandle
				);

				// 半透明用のパイプラインステートを追加
				{
					const RHI_PipelineState* pipelineState = resourceManager->FindPipelineState(
						passData->m_pipelineStateHandle
					);

					commandRecorder->SetPipelineState(
						pipelineState
					);
				}

				// パケットをループさせる
				for (u64 packetIndex = 0u; packetIndex < renderPackets.size(); ++packetIndex)
				{
					const RenderPacket& renderPacket = renderPackets[packetIndex];

					// シェーダーバインディング
					const auto shaderBindings = resourceRecorder->AllocateObject<RHI_ShaderBindings>();

					// シーンの定数バッファ
					{
						shaderBindings->SetConstantBuffer(
							0u,
							sceneConstantsBuffer
						);
					}

					// 頂点バッファ
					{
						const RHI_Buffer* vertexBuffer = resourceManager->FindBuffer(
							renderPacket.m_vertexBufferHandle
						);

						const auto vertexBufferArray = resourceRecorder->AllocateArray<const RHI_Buffer*>();
						vertexBufferArray[0] = vertexBuffer;

						commandRecorder->SetVertexBuffers(
							0u,
							1u,
							vertexBufferArray
						);
					}

					// インデックスバッファ
					{
						const RHI_Buffer* indexBuffer = resourceManager->FindBuffer(
							renderPacket.m_indexBufferHandle
						);

						RHI_Format indexBufferFormat = RHI_Format::Unknown;
						switch (indexBuffer->GetStride())
						{
						case 1: // u8
							indexBufferFormat = RHI_Format::R8_UInt;
							break;
						case 2: // u16
							indexBufferFormat = RHI_Format::R16_UInt;
							break;
						case 4: // u32
							indexBufferFormat = RHI_Format::R32_UInt;
							break;
						}

						commandRecorder->SetIndexBuffer(
							indexBufferFormat,
							indexBuffer
						);
					}

					// シェーダーバインディング
					{
						// マテリアル
						{
							// 定数バッファ
							{
								const RHI_Buffer* materialConstantBuffer = resourceManager->FindBuffer(
									renderPacket.m_materialConstantBufferHandle
								);

								shaderBindings->SetConstantBuffer(
									2u,
									materialConstantBuffer
								);
							}

							// ベースカラー
							if (renderPacket.m_baseColorTextureHandle.IsValid())
							{
								const RHI_TextureView* baseColorTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_baseColorTextureHandle
								);

								shaderBindings->SetTextureView(
									0u,
									baseColorTextureView
								);
							}

							// メタリックラフネス
							if (renderPacket.m_metallicRoughnessTextureHandle.IsValid())
							{
								const RHI_TextureView* metallicRoughnessTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_metallicRoughnessTextureHandle
								);

								shaderBindings->SetTextureView(
									1u,
									metallicRoughnessTextureView
								);
							}

							// ノーマル
							if (renderPacket.m_normalTextureHandle.IsValid())
							{
								const RHI_TextureView* normalTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_normalTextureHandle
								);

								shaderBindings->SetTextureView(
									2u,
									normalTextureView
								);
							}

							// オクルージョン
							if (renderPacket.m_occlusionTextureHandle.IsValid())
							{
								const RHI_TextureView* occlusionTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_occlusionTextureHandle
								);

								shaderBindings->SetTextureView(
									3u,
									occlusionTextureView
								);
							}

							// エミッシブ
							if (renderPacket.m_emissiveTextureHandle.IsValid())
							{
								const RHI_TextureView* emissiveTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_emissiveTextureHandle
								);

								shaderBindings->SetTextureView(
									4u,
									emissiveTextureView
								);
							}
						}
					}

					// オブジェクト定数バッファ
					{
						// オブジェクトの定数バッファを更新する
						const RHI_Buffer* objectConstantBuffer = resourceManager->FindBuffer(
							passData->m_objectConstantsBufferHandle
						);

						auto objectConstants = resourceRecorder->AllocateArray<ObjectConstants>();
						objectConstants->m_worldTransform = renderPacket.m_worldTransform * Matrix::Translation(Vector3(0.0f, 0.0f, 100.0f));

						commandRecorder->UpdateConstantBuffer(
							objectConstants,
							sizeof(ObjectConstants),
							objectConstantBuffer
						);

						// バインド
						shaderBindings->SetConstantBuffer(
							1u,
							objectConstantBuffer
						);
					}

					// シェーダーバインディングを設定
					{
						commandRecorder->SetShaderBindings(
							shaderBindings
						);
					}

					// 描画コマンド
					{
						commandRecorder->DrawIndexed(
							renderPacket.m_indexCount,
							renderPacket.m_startIndexLocation,
							0u
						);
					}
				}
			}

			static void ExecuteTransparentRenderPassJob3(void* data)
			{
				ZoneScopedNC("TransparentRenderPassJob3", tracy::Color::Orange);

				// パスデータの取得
				auto passData = reinterpret_cast<const TransparentRenderPassData*>(data);

				// 必要なリソースデータの取得
				const std::vector<RenderPacket>& renderPackets = *passData->m_renderPackets;
				const resource::RenderResourceManager* resourceManager = passData->m_renderResourceManager;
				RenderResourceRecorder* resourceRecorder = passData->m_resourceRecorder;
				RenderCommandRecorder* commandRecorder = passData->m_recorder;

				// シーン定数バッファを設定する
				const RHI_Buffer* sceneConstantsBuffer = resourceManager->FindBuffer(
					passData->m_sceneConstantsBufferHandle
				);

				// 半透明用のパイプラインステートを追加
				{
					const RHI_PipelineState* pipelineState = resourceManager->FindPipelineState(
						passData->m_pipelineStateHandle
					);

					commandRecorder->SetPipelineState(
						pipelineState
					);
				}

				// パケットをループさせる
				for (u64 packetIndex = 0u; packetIndex < renderPackets.size(); ++packetIndex)
				{
					const RenderPacket& renderPacket = renderPackets[packetIndex];

					// シェーダーバインディング
					const auto shaderBindings = resourceRecorder->AllocateObject<RHI_ShaderBindings>();

					// シーンの定数バッファ
					{
						shaderBindings->SetConstantBuffer(
							0u,
							sceneConstantsBuffer
						);
					}

					// 頂点バッファ
					{
						const RHI_Buffer* vertexBuffer = resourceManager->FindBuffer(
							renderPacket.m_vertexBufferHandle
						);

						const auto vertexBufferArray = resourceRecorder->AllocateArray<const RHI_Buffer*>();
						vertexBufferArray[0] = vertexBuffer;

						commandRecorder->SetVertexBuffers(
							0u,
							1u,
							vertexBufferArray
						);
					}

					// インデックスバッファ
					{
						const RHI_Buffer* indexBuffer = resourceManager->FindBuffer(
							renderPacket.m_indexBufferHandle
						);

						RHI_Format indexBufferFormat = RHI_Format::Unknown;
						switch (indexBuffer->GetStride())
						{
						case 1: // u8
							indexBufferFormat = RHI_Format::R8_UInt;
							break;
						case 2: // u16
							indexBufferFormat = RHI_Format::R16_UInt;
							break;
						case 4: // u32
							indexBufferFormat = RHI_Format::R32_UInt;
							break;
						}

						commandRecorder->SetIndexBuffer(
							indexBufferFormat,
							indexBuffer
						);
					}

					// シェーダーバインディング
					{
						// マテリアル
						{
							// 定数バッファ
							{
								const RHI_Buffer* materialConstantBuffer = resourceManager->FindBuffer(
									renderPacket.m_materialConstantBufferHandle
								);

								shaderBindings->SetConstantBuffer(
									2u,
									materialConstantBuffer
								);
							}

							// ベースカラー
							if (renderPacket.m_baseColorTextureHandle.IsValid())
							{
								const RHI_TextureView* baseColorTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_baseColorTextureHandle
								);

								shaderBindings->SetTextureView(
									0u,
									baseColorTextureView
								);
							}

							// メタリックラフネス
							if (renderPacket.m_metallicRoughnessTextureHandle.IsValid())
							{
								const RHI_TextureView* metallicRoughnessTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_metallicRoughnessTextureHandle
								);

								shaderBindings->SetTextureView(
									1u,
									metallicRoughnessTextureView
								);
							}

							// ノーマル
							if (renderPacket.m_normalTextureHandle.IsValid())
							{
								const RHI_TextureView* normalTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_normalTextureHandle
								);

								shaderBindings->SetTextureView(
									2u,
									normalTextureView
								);
							}

							// オクルージョン
							if (renderPacket.m_occlusionTextureHandle.IsValid())
							{
								const RHI_TextureView* occlusionTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_occlusionTextureHandle
								);

								shaderBindings->SetTextureView(
									3u,
									occlusionTextureView
								);
							}

							// エミッシブ
							if (renderPacket.m_emissiveTextureHandle.IsValid())
							{
								const RHI_TextureView* emissiveTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_emissiveTextureHandle
								);

								shaderBindings->SetTextureView(
									4u,
									emissiveTextureView
								);
							}
						}
					}

					// オブジェクト定数バッファ
					{
						// オブジェクトの定数バッファを更新する
						const RHI_Buffer* objectConstantBuffer = resourceManager->FindBuffer(
							passData->m_objectConstantsBufferHandle
						);

						auto objectConstants = resourceRecorder->AllocateArray<ObjectConstants>();
						objectConstants->m_worldTransform = renderPacket.m_worldTransform * Matrix::Translation(Vector3(0.0f, 0.0f, 200.0f));

						commandRecorder->UpdateConstantBuffer(
							objectConstants,
							sizeof(ObjectConstants),
							objectConstantBuffer
						);

						// バインド
						shaderBindings->SetConstantBuffer(
							1u,
							objectConstantBuffer
						);
					}

					// シェーダーバインディングを設定
					{
						commandRecorder->SetShaderBindings(
							shaderBindings
						);
					}

					// 描画コマンド
					{
						commandRecorder->DrawIndexed(
							renderPacket.m_indexCount,
							renderPacket.m_startIndexLocation,
							0u
						);
					}
				}
			}

			static void ExecuteTransparentRenderPassJob4(void* data)
			{
				ZoneScopedNC("TransparentRenderPassJob4", tracy::Color::Orange);

				// パスデータの取得
				auto passData = reinterpret_cast<const TransparentRenderPassData*>(data);

				// 必要なリソースデータの取得
				const std::vector<RenderPacket>& renderPackets = *passData->m_renderPackets;
				const resource::RenderResourceManager* resourceManager = passData->m_renderResourceManager;
				RenderResourceRecorder* resourceRecorder = passData->m_resourceRecorder;
				RenderCommandRecorder* commandRecorder = passData->m_recorder;

				// シーン定数バッファを設定する
				const RHI_Buffer* sceneConstantsBuffer = resourceManager->FindBuffer(
					passData->m_sceneConstantsBufferHandle
				);

				// 半透明用のパイプラインステートを追加
				{
					const RHI_PipelineState* pipelineState = resourceManager->FindPipelineState(
						passData->m_pipelineStateHandle
					);

					commandRecorder->SetPipelineState(
						pipelineState
					);
				}

				// パケットをループさせる
				for (u64 packetIndex = 0u; packetIndex < renderPackets.size(); ++packetIndex)
				{
					const RenderPacket& renderPacket = renderPackets[packetIndex];

					// シェーダーバインディング
					const auto shaderBindings = resourceRecorder->AllocateObject<RHI_ShaderBindings>();

					// シーンの定数バッファ
					{
						shaderBindings->SetConstantBuffer(
							0u,
							sceneConstantsBuffer
						);
					}

					// 頂点バッファ
					{
						const RHI_Buffer* vertexBuffer = resourceManager->FindBuffer(
							renderPacket.m_vertexBufferHandle
						);

						const auto vertexBufferArray = resourceRecorder->AllocateArray<const RHI_Buffer*>();
						vertexBufferArray[0] = vertexBuffer;

						commandRecorder->SetVertexBuffers(
							0u,
							1u,
							vertexBufferArray
						);
					}

					// インデックスバッファ
					{
						const RHI_Buffer* indexBuffer = resourceManager->FindBuffer(
							renderPacket.m_indexBufferHandle
						);

						RHI_Format indexBufferFormat = RHI_Format::Unknown;
						switch (indexBuffer->GetStride())
						{
						case 1: // u8
							indexBufferFormat = RHI_Format::R8_UInt;
							break;
						case 2: // u16
							indexBufferFormat = RHI_Format::R16_UInt;
							break;
						case 4: // u32
							indexBufferFormat = RHI_Format::R32_UInt;
							break;
						}

						commandRecorder->SetIndexBuffer(
							indexBufferFormat,
							indexBuffer
						);
					}

					// シェーダーバインディング
					{
						// マテリアル
						{
							// 定数バッファ
							{
								const RHI_Buffer* materialConstantBuffer = resourceManager->FindBuffer(
									renderPacket.m_materialConstantBufferHandle
								);

								shaderBindings->SetConstantBuffer(
									2u,
									materialConstantBuffer
								);
							}

							// ベースカラー
							if (renderPacket.m_baseColorTextureHandle.IsValid())
							{
								const RHI_TextureView* baseColorTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_baseColorTextureHandle
								);

								shaderBindings->SetTextureView(
									0u,
									baseColorTextureView
								);
							}

							// メタリックラフネス
							if (renderPacket.m_metallicRoughnessTextureHandle.IsValid())
							{
								const RHI_TextureView* metallicRoughnessTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_metallicRoughnessTextureHandle
								);

								shaderBindings->SetTextureView(
									1u,
									metallicRoughnessTextureView
								);
							}

							// ノーマル
							if (renderPacket.m_normalTextureHandle.IsValid())
							{
								const RHI_TextureView* normalTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_normalTextureHandle
								);

								shaderBindings->SetTextureView(
									2u,
									normalTextureView
								);
							}

							// オクルージョン
							if (renderPacket.m_occlusionTextureHandle.IsValid())
							{
								const RHI_TextureView* occlusionTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_occlusionTextureHandle
								);

								shaderBindings->SetTextureView(
									3u,
									occlusionTextureView
								);
							}

							// エミッシブ
							if (renderPacket.m_emissiveTextureHandle.IsValid())
							{
								const RHI_TextureView* emissiveTextureView = resourceManager->FindTextureDefaultView(
									renderPacket.m_emissiveTextureHandle
								);

								shaderBindings->SetTextureView(
									4u,
									emissiveTextureView
								);
							}
						}
					}

					// オブジェクト定数バッファ
					{
						// オブジェクトの定数バッファを更新する
						const RHI_Buffer* objectConstantBuffer = resourceManager->FindBuffer(
							passData->m_objectConstantsBufferHandle
						);

						auto objectConstants = resourceRecorder->AllocateArray<ObjectConstants>();
						objectConstants->m_worldTransform = renderPacket.m_worldTransform * Matrix::Translation(Vector3(0.0f, 0.0f, 300.0f));

						commandRecorder->UpdateConstantBuffer(
							objectConstants,
							sizeof(ObjectConstants),
							objectConstantBuffer
						);

						// バインド
						shaderBindings->SetConstantBuffer(
							1u,
							objectConstantBuffer
						);
					}

					// シェーダーバインディングを設定
					{
						commandRecorder->SetShaderBindings(
							shaderBindings
						);
					}

					// 描画コマンド
					{
						commandRecorder->DrawIndexed(
							renderPacket.m_indexCount,
							renderPacket.m_startIndexLocation,
							0u
						);
					}
				}
			}

			TransparentRenderPass::TransparentRenderPass(
				PipelineStateResourceHandle pipelineStateHandle
			) : m_data(), m_pipelineStateHandle(pipelineStateHandle)
			{
			}
			
			bool TransparentRenderPass::Create(const RenderPassDesc& desc)
			{
				m_data.resize(4u);

				return RenderPass::Create(desc);
			}

			bool TransparentRenderPass::Destroy()
			{
				return RenderPass::Destroy();
			}

			void TransparentRenderPass::DispatchJobs(
				const RenderContext& context,
				thread::RenderJobSystem& renderJobSystem,
				RenderCommandRecorder** commandRecorders,
				RenderResourceRecorder** resourceRecorders,
				const RenderPacketsBundle& bundle,
				std::latch* passLatch
			)
			{
				for (u64 i = 0u; i < m_data.size(); ++i)
				{
					TransparentRenderPassData& data = m_data[i];
					data.m_recorder = commandRecorders[i];
					data.m_resourceRecorder = resourceRecorders[i];
					data.m_renderResourceManager = context.m_renderResourceManager;
					data.m_renderPackets = &bundle.m_transparentRenderPackets;
					data.m_pipelineStateHandle = m_pipelineStateHandle;
					data.m_sceneConstantsBufferHandle = context.m_sceneConstantsBufferHandle;
					data.m_objectConstantsBufferHandle = context.m_objectConstantsBufferHandle;

					thread::RenderJob job = {};
					switch (i)
					{
					case 0:
						job.m_execute = ExecuteTransparentRenderPassJob1;
						break;
					case 1:
						job.m_execute = ExecuteTransparentRenderPassJob2;
						break;
					case 2:
						job.m_execute = ExecuteTransparentRenderPassJob3;
						break;
					case 3:
						job.m_execute = ExecuteTransparentRenderPassJob4;
						break;
					}

					job.m_jobData = &data;
					job.m_latch = passLatch;

					renderJobSystem.PushJob(job);
				}
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts