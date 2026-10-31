// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/Debug/DebugRenderPass.hpp"

#include "Graphics/RenderCommand/RenderCommandRecorder.hpp"
#include "Graphics/RenderCommand/RenderResourceRecorder.hpp"

#include "Graphics/RenderResourceManager.hpp"

#include "Graphics/RHI/RHI_Buffer.hpp"
#include "Graphics/RHI/RHI_ShaderBindings.hpp"

#include "Core/Thread/RenderJobSystem.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			static void ExecuteDebugRenderPassJob(void* data)
			{
				ZoneScopedNC("DebugRenderPass", tracy::Color::Green);

				auto passData = reinterpret_cast<DebugRenderPassData*>(data);

				// 簡易のためローカル変数に代入する
				const resource::RenderResourceManager* resourceManager = passData->m_renderResourceManager;
				RenderCommandRecorder* recorder = passData->m_recorder;
				RenderResourceRecorder* resourceRecorder = passData->m_resourceRecorder;
				const BufferResourceHandle sceneCBHandle = passData->m_sceneConstantBufferHandle;
				const BufferResourceHandle vbHandle = passData->m_lineVertexBufferHandle;
				const PipelineStateResourceHandle stateHandle = passData->m_stateHandle;
			
				// シーン定数バッファのバインド
				{
					const RHI_Buffer* sceneConstantBuffer = resourceManager->FindBuffer(
						sceneCBHandle
					);

					// シェーダーバインディング
					const auto shaderBindings = resourceRecorder->AllocateObject<RHI_ShaderBindings>();
					shaderBindings->SetConstantBuffer(
						0u, sceneConstantBuffer
					);

					recorder->SetShaderBindings(shaderBindings);
				}

				// 頂点バッファのバインド
				const RHI_Buffer* vb = resourceManager->FindBuffer(vbHandle);
				{
					const auto vbArray = resourceRecorder->AllocateArray<const RHI_Buffer*>();
					vbArray[0] = vb;

					recorder->SetVertexBuffers(0u, 1u, vbArray);
				}

				// パイプラインステートのバインド
				{
					const RHI_PipelineState* state = resourceManager->FindPipelineState(
						stateHandle
					);

					recorder->SetPipelineState(state);
				}

				recorder->Draw(
					static_cast<u32>(vb->GetCount()),
					0u
				);
			}

			DebugRenderPass::DebugRenderPass(
				BufferResourceHandle vertexBufferHandle,
				PipelineStateResourceHandle stateHandle
			) : m_data(), 
				m_lineVertexBufferHandle(vertexBufferHandle),
				m_stateHandle(stateHandle)
			{
			}

			bool DebugRenderPass::Create(const RenderPassDesc& desc)
			{
				if (!RenderPass::Create(desc))
				{
					return false;
				}
				m_data.resize(1u);

				return true;
			}

			bool DebugRenderPass::Destroy()
			{
				return RenderPass::Destroy();
			}

			void DebugRenderPass::DispatchJobs(
				const RenderContext& context,
				thread::RenderJobSystem& renderJobSystem,
				RenderCommandRecorder** commandRecorders,
				RenderResourceRecorder** resourceRecorders,
				const RenderPacketsBundle& bundle,
				std::latch* passLatch
			)
			{
				DebugRenderPassData& data = m_data[0];
				data.m_recorder = commandRecorders[0];
				data.m_resourceRecorder = resourceRecorders[0];
				data.m_renderResourceManager = context.m_renderResourceManager;
				data.m_sceneConstantBufferHandle = context.m_sceneConstantsBufferHandle;
				data.m_lineVertexBufferHandle = m_lineVertexBufferHandle;
				data.m_stateHandle = m_stateHandle;

				thread::RenderJob job = {};
				job.m_execute = ExecuteDebugRenderPassJob;
				job.m_jobData = &data;
				job.m_latch = passLatch;

				renderJobSystem.PushJob(job);
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts