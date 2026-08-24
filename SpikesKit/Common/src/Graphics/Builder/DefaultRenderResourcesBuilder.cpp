// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/Builder/DefaultRenderResourcesBuilder.hpp"

#include "Graphics/RenderResourceManager.hpp"

#include "Graphics/RHI/RHI_PipelineState.hpp"
#include "Graphics/RHI/RHI_ShaderBindingsLayout.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			DefaultRenderResourcesBuilder::DefaultRenderResourcesBuilder() :
				m_pointSamplerHandle(),
				m_linearSamplerHandle(),
				m_anisotropicSamplerHandle(),
				m_meshShaderBindingsLayoutHandle(),
				m_opaquePipelineStateHandle(),
				m_maskPipelineStateHandle(),
				m_transparentPipelineStateHandle()
			{
			}

			bool DefaultRenderResourcesBuilder::Build(
				const DefaultRenderResourcesBuilderDesc& desc
			)
			{
				using namespace resource;

				RenderResourceManager* resourceManager = desc.m_renderResourceManager;

				// CommonShaders
				const RHI_Shader* staticMeshVertexShader = resourceManager->FindShader(
					desc.m_staticMeshVertexShaderHandle
				);
				const RHI_Shader* meshPBRPixelShader = resourceManager->FindShader(
					desc.m_meshPBRPixelShaderHandle
				);
				const RHI_Shader* meshPBRWithPixelShader = resourceManager->FindShader(
					desc.m_meshPBRWithMaskPixelShaderHandle
				);

				// Samplers
				{
					m_pointSamplerHandle = resourceManager->CreateOrGetSampler(RHI_SamplerState::GetPoint());
					m_linearSamplerHandle = resourceManager->CreateOrGetSampler(RHI_SamplerState::GetLinear());
					m_anisotropicSamplerHandle = resourceManager->CreateOrGetSampler(RHI_SamplerState::GetAnisotropic());
				}

				// ShaderBindingsLayout
				{
					RHI_ShaderBindingsLayoutDesc meshShaderBindingsLayoutDesc = {};
					meshShaderBindingsLayoutDesc.m_psSamplers[0] = resourceManager->FindSampler(
						m_pointSamplerHandle
					);
					meshShaderBindingsLayoutDesc.m_psSamplers[1] = resourceManager->FindSampler(
						m_linearSamplerHandle
					);
					meshShaderBindingsLayoutDesc.m_psSamplers[2] = resourceManager->FindSampler(
						m_anisotropicSamplerHandle
					);

					m_meshShaderBindingsLayoutHandle = resourceManager->CreateShaderBindingsLayout(
						meshShaderBindingsLayoutDesc
					);
				}

				// PipelineState
				{
					// Opaque
					{
						RHI_PipelineStateDesc opaquePipelineStateDesc = {};
						opaquePipelineStateDesc.m_rtvFormat = RHI_Format::R8G8B8A8_UNorm;
						opaquePipelineStateDesc.m_dsvFormat = RHI_Format::D24_UNorm_S8_UInt;
						opaquePipelineStateDesc.m_rasterizerState = RHI_RasterizerState::GetFillSolidCullBack();
						opaquePipelineStateDesc.m_primitiyTopology = RHI_PrimitiveTopology::TriangleList;
						opaquePipelineStateDesc.m_blendState = RHI_BlendState::GetNoBlend();
						opaquePipelineStateDesc.m_depthStencilState = RHI_DepthStencilState::GetDepthOnStencilOn();
						opaquePipelineStateDesc.m_vertexShader = staticMeshVertexShader;
						opaquePipelineStateDesc.m_pixelShader = meshPBRPixelShader;
						opaquePipelineStateDesc.m_shaderBindingsLayout = resourceManager->FindShaderBindingsLayout(
							m_meshShaderBindingsLayoutHandle
						);
						opaquePipelineStateDesc.m_vertexLayouts =
						{
							{ "POSITION", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "NORMAL", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TANGENT", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TEXCOORD", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float },
							{ "TEXCOORD", 1u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float }
						};

						m_opaquePipelineStateHandle = resourceManager->CreateOrGetPipelineState(
							opaquePipelineStateDesc
						);
					}

					// Mask
					{
						RHI_PipelineStateDesc maskPipelineStateDesc = {};
						maskPipelineStateDesc.m_rtvFormat = RHI_Format::R8G8B8A8_UNorm;
						maskPipelineStateDesc.m_dsvFormat = RHI_Format::D24_UNorm_S8_UInt;
						maskPipelineStateDesc.m_rasterizerState = RHI_RasterizerState::GetFillSolidCullBack();
						maskPipelineStateDesc.m_primitiyTopology = RHI_PrimitiveTopology::TriangleList;
						maskPipelineStateDesc.m_blendState = RHI_BlendState::GetNoBlend();
						maskPipelineStateDesc.m_depthStencilState = RHI_DepthStencilState::GetDepthOnStencilOn();
						maskPipelineStateDesc.m_vertexShader = staticMeshVertexShader;
						maskPipelineStateDesc.m_pixelShader = meshPBRWithPixelShader;
						maskPipelineStateDesc.m_shaderBindingsLayout = resourceManager->FindShaderBindingsLayout(
							m_meshShaderBindingsLayoutHandle
						);
						maskPipelineStateDesc.m_vertexLayouts =
						{
							{ "POSITION", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "NORMAL", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TANGENT", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TEXCOORD", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float },
							{ "TEXCOORD", 1u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float }
						};

						m_maskPipelineStateHandle = resourceManager->CreateOrGetPipelineState(
							maskPipelineStateDesc
						);
					}

					// Transparent
					{
						RHI_PipelineStateDesc transparentPipelineStateDesc = {};
						transparentPipelineStateDesc.m_rtvFormat = RHI_Format::R8G8B8A8_UNorm;
						transparentPipelineStateDesc.m_dsvFormat = RHI_Format::D24_UNorm_S8_UInt;
						transparentPipelineStateDesc.m_rasterizerState = RHI_RasterizerState::GetFillSolidCullBack();
						transparentPipelineStateDesc.m_primitiyTopology = RHI_PrimitiveTopology::TriangleList;
						transparentPipelineStateDesc.m_blendState = RHI_BlendState::GetAlphaBlend();
						transparentPipelineStateDesc.m_depthStencilState = RHI_DepthStencilState::GetDeptnOnWriteOffStencilOff();
						transparentPipelineStateDesc.m_vertexShader = staticMeshVertexShader;
						transparentPipelineStateDesc.m_pixelShader = meshPBRPixelShader;
						transparentPipelineStateDesc.m_shaderBindingsLayout = resourceManager->FindShaderBindingsLayout(
							m_meshShaderBindingsLayoutHandle
						);
						transparentPipelineStateDesc.m_vertexLayouts =
						{
							{ "POSITION", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "NORMAL", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TANGENT", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32B32A32_Float },
							{ "TEXCOORD", 0u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float },
							{ "TEXCOORD", 1u, 0u, 0xFFFFFFFF, RHI_Format::R32G32_Float }
						};

						m_transparentPipelineStateHandle = resourceManager->CreateOrGetPipelineState(
							transparentPipelineStateDesc
						);
					}
				}

				return true;
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts