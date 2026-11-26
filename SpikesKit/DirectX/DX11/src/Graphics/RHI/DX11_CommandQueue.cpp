// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/RHI_ShaderBindings.hpp"

#include "Graphics/RHI/DX11_CommandQueue.hpp"
#include "Graphics/RHI/DX11_Device.hpp"
#include "Graphics/RHI/DX11_TextureView.hpp"
#include "Graphics/RHI/DX11_Shader.hpp"
#include "Graphics/RHI/DX11_ShaderBindingsLayout.hpp"
#include "Graphics/RHI/DX11_PipelineState.hpp"
#include "Graphics/RHI/DX11_Buffer.hpp"
#include "Graphics/RHI/DX11_Format.hpp"
#include "Graphics/RHI/DX11_Sampler.hpp"

#include "Graphics/RenderCommand/RenderCommand.hpp"
#include "Graphics/RenderCommand/RenderCommandBuffer.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_CommandQueue::DX11_CommandQueue() : graphics::RHI_CommandQueue(),
				m_context()
			{
			}

			bool DX11_CommandQueue::Create(
				graphics::RHI_Device* device,
				graphics::CommandQueueType type
			)
			{
				(void)device;
				(void)type;

				return true;
			}

			bool DX11_CommandQueue::Destroy()
			{
				Release();
				
				return true;
			}
			
			void DX11_CommandQueue::Signal(graphics::RHI_Fence* fence, u64 value)
			{
			}
			
			void DX11_CommandQueue::Wait(graphics::RHI_Fence* fence, u64 value)
			{
			}

			void DX11_CommandQueue::Execute(const std::vector<graphics::RenderCommandBuffer>& commandBuffers)
			{
				const u64 renderCommandSize = sizeof(graphics::RenderCommand);

				for (u64 bufferIndex = 0u; bufferIndex < commandBuffers.size(); ++bufferIndex)
				{
					const graphics::RenderCommandBuffer& commandBuffer = commandBuffers[bufferIndex];
					const u8* bufferStart = commandBuffer.GetBufferStart();

					for (u32 commandIndex = 0u; commandIndex < commandBuffer.GetCommandCount(); ++commandIndex)
					{
						const auto renderCommand = reinterpret_cast<const graphics::RenderCommand*>(
							bufferStart + (commandIndex * renderCommandSize)
						);

						switch (renderCommand->m_type)
						{
						case graphics::RenderCommandType::TransitonResourceState:
						{
							// なにもしない
						}
						break;
						case graphics::RenderCommandType::ClearRenderTargetView:
						{
							ID3D11RenderTargetView* rtv = static_cast<DX11_TextureView*>(
								renderCommand->m_clearRenderTargetView.m_rtv
								)->GetRenderTargetView();

							m_context->ClearRenderTargetView(
								rtv,
								&renderCommand->m_clearRenderTargetView.m_clearColor.x
							);
						}
						break;
						case graphics::RenderCommandType::ClearDepthStencilView:
						{
							ID3D11DepthStencilView* dsv = static_cast<DX11_TextureView*>(
								renderCommand->m_clearDepthStencilView.m_dsv
								)->GetDepthStencilView();

							m_context->ClearDepthStencilView(
								dsv,
								internal::ToD3D11ClearFlag(
									renderCommand->m_clearDepthStencilView.m_clearFlags
								),
								renderCommand->m_clearDepthStencilView.m_depth,
								renderCommand->m_clearDepthStencilView.m_stencil
							);
						}
						break;
						case graphics::RenderCommandType::SetRenderTargets:
						{
							// レンダーターゲットビュー
							std::array<ID3D11RenderTargetView*, 8u> renderTargetViews = {};
							const u32 numRTVs = renderCommand->m_setRenderTargets.m_numRTVS;
							for (u32 renderTargetViewIndex = 0u; renderTargetViewIndex < numRTVs; ++renderTargetViewIndex)
							{
								renderTargetViews[renderTargetViewIndex] = static_cast<DX11_TextureView*>(
									renderCommand->m_setRenderTargets.m_rtvs[renderTargetViewIndex]
									)->GetRenderTargetView();
							}

							// デプスステンシルビュー
							ID3D11DepthStencilView* depthStencilView = static_cast<DX11_TextureView*>(
								renderCommand->m_setRenderTargets.m_dsv
								)->GetDepthStencilView();

							m_context->OMSetRenderTargets(
								numRTVs,
								renderTargetViews.data(),
								depthStencilView
							);
						}
						break;
						case graphics::RenderCommandType::SetViewports:
						{
							TS_ASSERT(renderCommand->m_setViewports.m_viewports[0], "Viewportがnullptrです");

							std::array<D3D11_VIEWPORT, 8u> viewports = {};
							for (u32 i = 0u; i < renderCommand->m_setViewports.m_numViewports; ++i)
							{
								viewports[i].TopLeftX = renderCommand->m_setViewports.m_viewports[i]->m_topLeft.x;
								viewports[i].TopLeftY = renderCommand->m_setViewports.m_viewports[i]->m_topLeft.y;
								viewports[i].Width = renderCommand->m_setViewports.m_viewports[i]->m_size.x;
								viewports[i].Height = renderCommand->m_setViewports.m_viewports[i]->m_size.y;
								viewports[i].MinDepth = renderCommand->m_setViewports.m_viewports[i]->m_minDepth;
								viewports[i].MaxDepth = renderCommand->m_setViewports.m_viewports[i]->m_maxDepth;
							}

							m_context->RSSetViewports(
								renderCommand->m_setViewports.m_numViewports,
								viewports.data()
							);
						}
						break;
						case graphics::RenderCommandType::SetShaderBindings:
						{
							const graphics::RHI_ShaderBindings* shaderBindings = renderCommand->m_setShaderBindings.m_shaderBindings;

							// Common
							using Visibility = graphics::RHI_ShaderBindingsVisibility;

							// ConstantBuffer
							using ConstantBuffers = graphics::RHI_ShaderBindingsState::ConstantBuffers;

							// VS
							{
								const ConstantBuffers& vsConstantBuffers = shaderBindings->GetState().m_vsConstantBuffers;

								if (vsConstantBuffers.m_minSlot != graphics::k_maxCBVSlots)
								{
									const u32 numBuffers = (vsConstantBuffers.m_maxSlot - vsConstantBuffers.m_minSlot) + 1u;

									ID3D11Buffer* buffers[graphics::k_maxCBVSlots] = {};
									for (u32 i = 0u; i < numBuffers; ++i)
									{
										if (vsConstantBuffers.m_buffers[vsConstantBuffers.m_minSlot + i])
										{
											buffers[i] = static_cast<const DX11_Buffer*>(vsConstantBuffers.m_buffers[vsConstantBuffers.m_minSlot + i])->GetBuffer();
										}
									}

									m_context->VSSetConstantBuffers(
										vsConstantBuffers.m_minSlot,
										numBuffers,
										buffers
									);
								}
							}

							// PS
							{
								const ConstantBuffers& psConstantBuffers = shaderBindings->GetState().m_psConstantBuffers;

								if (psConstantBuffers.m_minSlot != graphics::k_maxCBVSlots)
								{
									const u32 numBuffers = (psConstantBuffers.m_maxSlot - psConstantBuffers.m_minSlot) + 1u;

									ID3D11Buffer* buffers[graphics::k_maxCBVSlots] = {};
									for (u32 i = 0u; i < numBuffers; ++i)
									{
										if (psConstantBuffers.m_buffers[psConstantBuffers.m_minSlot + i])
										{
											buffers[i] = static_cast<const DX11_Buffer*>(psConstantBuffers.m_buffers[psConstantBuffers.m_minSlot + i])->GetBuffer();
										}
									}

									m_context->PSSetConstantBuffers(
										psConstantBuffers.m_minSlot,
										numBuffers,
										buffers
									);
								}
							}

							// TextureViews
							using TextureViews = graphics::RHI_ShaderBindingsState::TextureViews;
							
							// PS
							{
								const TextureViews& psTextureViews = shaderBindings->GetState().m_psTextureViews;

								if (psTextureViews.m_minSlot != graphics::k_maxSRVSlots)
								{
									const u32 numViews = (psTextureViews.m_maxSlot - psTextureViews.m_minSlot) + 1u;

									ID3D11ShaderResourceView* views[graphics::k_maxSRVSlots] = {};
									for (u32 i = 0u; i < numViews; ++i)
									{
										if (psTextureViews.m_views[psTextureViews.m_minSlot + i])
										{
											views[i] = static_cast<const DX11_TextureView*>(psTextureViews.m_views[psTextureViews.m_minSlot + i])->GetShaderResourceView();
										}
									}

									m_context->PSSetShaderResources(
										psTextureViews.m_minSlot,
										numViews,
										views
									);
								}
							}
						}
						break;
						case graphics::RenderCommandType::SetPipelineState:
						{
							const auto pipeline = static_cast<const DX11_PipelineState*>(
								renderCommand->m_setPipelineState.m_pipelineState
							);

							// 頂点シェーダー
							{
								const auto inputLayout = pipeline->GetInputLayout();

								m_context->IASetInputLayout(
									inputLayout
								);

								const auto vertexShader = static_cast<const DX11_Shader*>(pipeline->GetVertexShader());
								if (vertexShader->GetType() != graphics::RHI_ShaderType::VertexShader)
								{
									TS_ASSERT(false, "頂点シェーダーではありません");
								}

								m_context->VSSetShader(
									vertexShader->GetVertexShader(),
									nullptr,
									0u
								);
							}

							// ピクセルシェーダー
							{
								const auto pixelShader = static_cast<const DX11_Shader*>(pipeline->GetPixelShader());
								if (pixelShader->GetType() != graphics::RHI_ShaderType::PixelShader)
								{
									TS_ASSERT(false, "ピクセルシェーダーではありません");
								}

								m_context->PSSetShader(
									pixelShader->GetPixelShader(),
									nullptr,
									0u
								);
							}

							// レイアウト(サンプラステートの設定)
							{
								const auto shaderBindingsLayout = static_cast<const DX11_ShaderBindingsLayout*>(
									pipeline->GetShaderBindingsLayout()
								);

								// PS
								{
									const std::array<const graphics::RHI_Sampler*, graphics::k_maxSamplersSlots>& samplers = shaderBindingsLayout->GetPSSamplers();
									
									std::array<ID3D11SamplerState*, graphics::k_maxSamplersSlots> dx11Samplers = {};
									for (u64 i = 0u; i < dx11Samplers.size(); ++i)
									{
										if (!samplers[i])
										{
											continue;
										}
											
										dx11Samplers[i] = reinterpret_cast<ID3D11SamplerState*>(samplers[i]->GetNativeSampler());
									}
									
									m_context->PSSetSamplers(
										0u,
										static_cast<u32>(dx11Samplers.size()),
										dx11Samplers.data()
									);
								}
							}

							// 描画ステート
							{
								switch (pipeline->GetPrimitiveTopology())
								{
								case graphics::RHI_PrimitiveTopology::PointList:
								{
									m_context->IASetPrimitiveTopology(
										D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
									);
								}
								break;
								case graphics::RHI_PrimitiveTopology::LineList:
								{
									m_context->IASetPrimitiveTopology(
										D3D11_PRIMITIVE_TOPOLOGY_LINELIST
									);
								}
								break;
								case graphics::RHI_PrimitiveTopology::TriangleList:
								{
									m_context->IASetPrimitiveTopology(
										D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
									);
								}
								break;
								}

								m_context->RSSetState(
									pipeline->GetRasterizerState()
								);

								m_context->OMSetDepthStencilState(
									pipeline->GetDepthStencilState(),
									0u
								);

								constexpr f32 factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
								m_context->OMSetBlendState(
									pipeline->GetBlendState(),
									factor,
									0xFFFFFFFF
								);
							}
						}
						break;
						case graphics::RenderCommandType::SetVertexBuffers:
						{
							// __assume(renderCommand->m_setVertexBuffers.m_vertexBuffers[0] != nullptr);

							TS_ASSERT(
								renderCommand->m_setVertexBuffers.m_vertexBuffers[0],
								"VertexBuffersがnullptrです。"
							);

							u32 strideInBytes = static_cast<u32>(renderCommand->m_setVertexBuffers.m_vertexBuffers[0]->GetStride());
							u32 offset = 0u;

							ID3D11Buffer* vertexBuffers[8] = {};
							for (u32 i = 0u; i < renderCommand->m_setVertexBuffers.m_numViews; ++i)
							{
								const auto dx11VB = static_cast<const DX11_Buffer*>(renderCommand->m_setVertexBuffers.m_vertexBuffers[i]);
								
								vertexBuffers[i] = dx11VB->GetBuffer();
							}

							m_context->IASetVertexBuffers(
								renderCommand->m_setVertexBuffers.m_startSlot,
								renderCommand->m_setVertexBuffers.m_numViews,
								vertexBuffers,
								&strideInBytes,
								&offset
							);
						}
						break;
						case graphics::RenderCommandType::SetIndexBuffer:
						{
							TS_ASSERT(
								renderCommand->m_setIndexBuffer.m_indexBuffer,
								"IndexBuffersがnullptrです。"
							);

							ID3D11Buffer* indexBuffer = nullptr;
							indexBuffer = static_cast<const DX11_Buffer*>(renderCommand->m_setIndexBuffer.m_indexBuffer)->GetBuffer();

							m_context->IASetIndexBuffer(
								indexBuffer,
								dx11::ToDXGIFormat(renderCommand->m_setIndexBuffer.m_format),
								0u
							);
						}
						break;
						case graphics::RenderCommandType::UpdateConstantBuffer:
						{
							TS_ASSERT(
								renderCommand->m_updateConstantBuffer.m_data,
								"Dataがnullptrです"
							);

							TS_ASSERT(
								renderCommand->m_updateConstantBuffer.m_constantBuffer,
								"ConstantBufferがnullptrです"
							);

							TS_ASSERT(
								renderCommand->m_updateConstantBuffer.m_constantBuffer->GetUsage() == graphics::RHI_BufferUsage::Dynamic,
								"Dynamicバッファでなければいけません"
							);

							const auto dx11ConstantBuffer = static_cast<const DX11_Buffer*>(
								renderCommand->m_updateConstantBuffer.m_constantBuffer
							);

							ID3D11Resource* resource = dx11ConstantBuffer->GetBuffer();

							D3D11_MAPPED_SUBRESOURCE subresource = {};
							HRESULT hr = m_context->Map(
								resource,
								0u,
								D3D11_MAP_WRITE_DISCARD,
								0u,
								&subresource
							);
							TS_ASSERT(hr == S_OK, "リソースのマップに失敗しました");

							auto mappedPtr = static_cast<u8*>(subresource.pData);
							std::memcpy(
								mappedPtr,
								renderCommand->m_updateConstantBuffer.m_data,
								renderCommand->m_updateConstantBuffer.m_dataSize
							);

							m_context->Unmap(
								resource,
								0u
							);
						}
						break;
						case graphics::RenderCommandType::UpdateSubresources:
						{
							TS_ASSERT(
								renderCommand->m_updateSubresources.m_buffer,
								"Bufferがnullptrです"
							);

							TS_ASSERT(
								renderCommand->m_updateSubresources.m_srcData,
								"srcDataがnullptrです"
							);

							ID3D11Resource* resource = nullptr;
							resource = static_cast<const DX11_Buffer*>(
								renderCommand->m_updateSubresources.m_buffer
							)->GetBuffer();

							m_context->UpdateSubresource(
								resource,
								0u,
								nullptr,
								renderCommand->m_updateSubresources.m_srcData,
								0u, //renderCommand->m_updateSubresources.m_srcDataSize,
								0u
							);
						}
						break;
						case graphics::RenderCommandType::Draw:
						{
							m_context->Draw(
								renderCommand->m_draw.m_vertexCount,
								renderCommand->m_draw.m_vertexStart
							);
						}
						break;
						case graphics::RenderCommandType::DrawInstanced:
						{
							m_context->DrawInstanced(
								renderCommand->m_drawInstanced.m_vertexCountPerInstance,
								renderCommand->m_drawInstanced.m_instanceCount,
								renderCommand->m_drawInstanced.m_startVertexLocation,
								renderCommand->m_drawInstanced.m_startInstanceLocation
							);
						}
						break;
						case graphics::RenderCommandType::DrawIndexed:
						{
							m_context->DrawIndexed(
								renderCommand->m_drawIndexed.m_indexCount,
								renderCommand->m_drawIndexed.m_startIndexLocation,
								renderCommand->m_drawIndexed.m_baseVertexLocation
							);
						}
						break;
						case graphics::RenderCommandType::DrawIndexedInstanced:
						{
							m_context->DrawIndexedInstanced(
								renderCommand->m_drawIndexedInstanced.m_indexCountPerInstance,
								renderCommand->m_drawIndexedInstanced.m_instanceCount,
								renderCommand->m_drawIndexedInstanced.m_startIndexLocation,
								renderCommand->m_drawIndexedInstanced.m_baseVertexLocation,
								renderCommand->m_drawIndexedInstanced.m_startInstanceLocation
							);
						}
						break;
						}
					}
				}
			}

			void DX11_CommandQueue::Release()
			{
				delete this;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts