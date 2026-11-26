// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RenderPipeline/ForwardRenderPipeline.hpp"
#include "Graphics/RenderCommand/RenderCommandBuffer.hpp"
#include "Graphics/RenderCommand/RenderCommandRecorder.hpp"

#include "Graphics/RenderPass/PresentationSetupRenderPass.hpp"
#include "Graphics/RenderPass/OpaqueRenderPass.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			ForwardRenderPipeline::ForwardRenderPipeline() : RenderPipeline()
			{
			}

			bool ForwardRenderPipeline::Create(
				RHI_Device* device,
				const std::vector<RenderCommandBuffer>& commandBuffers
			)
			{
				TS_ASSERT(commandBuffers.size() >= 2u, "コマンドバッファが不足しています");

				return true;
			}

			bool ForwardRenderPipeline::Destroy()
			{
				for (auto&& renderPass : m_renderPasses)
				{
					if (!renderPass->Destroy())
					{
						TS_FATAL_LOG("RenderPassの破棄に失敗しました。");

						return false;
					}

					TS_SAFE_RELEASE(renderPass);
				}

				return true;
			}

			void ForwardRenderPipeline::Render(
				const RenderContext& context,
				thread::RenderJobSystem& renderJobSystem,
				std::vector<RenderCommandRecorder>& commandRecorders,
				std::vector<RenderResourceRecorder>& resourceRecorders,
				const RenderPacketsBundle& bundle
			)
			{
				u64 allocatedJobCount = 0u;

				// Render
				for (u64 i = 0u; i < m_renderPasses.size(); ++i)
				{
					const u32 jobCount = m_renderPasses[i]->GetJobCount();
					if (jobCount == 0u)
					{
						continue;
					}
					TS_ASSERT(
						commandRecorders.size() >= allocatedJobCount + jobCount,
						"コマンドレコーダーが不足しています"
					);

					// 描画コマンドレコーダー
					RenderCommandRecorder* renderCommandRecorders[8u];
					for (u32 recorderIndex = 0u;
						recorderIndex < jobCount;
						++recorderIndex)
					{
						renderCommandRecorders[recorderIndex] = &commandRecorders[allocatedJobCount + recorderIndex];
					}

					// 描画リソースレコーダー
					RenderResourceRecorder* renderResourceRecorders[8u];
					for (u32 recorderIndex = 0u; recorderIndex < jobCount; ++recorderIndex)
					{
						renderResourceRecorders[recorderIndex] = &resourceRecorders[allocatedJobCount + recorderIndex];
					}

					std::latch passLatch(jobCount);
					m_renderPasses[i]->DispatchJobs(
						context,
						renderJobSystem,
						renderCommandRecorders,
						renderResourceRecorders,
						bundle,
						&passLatch
					);

					allocatedJobCount += jobCount;

					passLatch.wait();
				}
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts