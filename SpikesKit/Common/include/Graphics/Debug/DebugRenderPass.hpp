// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDER_PASS_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDER_PASS_HPP_

#include "../RenderPass/RenderPass.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct DebugRenderPassData final
			{
				RenderCommandRecorder* m_recorder{};
				RenderResourceRecorder* m_resourceRecorder{};
				
				const resource::RenderResourceManager* m_renderResourceManager{};

				BufferResourceHandle m_sceneConstantBufferHandle{};
				BufferResourceHandle m_lineVertexBufferHandle{};
				PipelineStateResourceHandle m_stateHandle{};
			};

			class DebugRenderPass final : public RenderPass
			{
			public:
				DebugRenderPass(
					BufferResourceHandle vertexBufferHandle,
					PipelineStateResourceHandle stateHandle
				);
				DebugRenderPass(const DebugRenderPass&) = delete;
				DebugRenderPass(DebugRenderPass&&) noexcept = default;
				~DebugRenderPass() noexcept override = default;
			public:
				[[nodiscard]]
				bool Create(
					const RenderPassDesc& desc
				) override;

				[[nodiscard]]
				bool Destroy() override;
			public:
				void DispatchJobs(
					const RenderContext& context,
					thread::RenderJobSystem& renderJobSystem,
					RenderCommandRecorder** commandRecorders,
					RenderResourceRecorder** resourceRecorders,
					const RenderPacketsBundle& bundle,
					std::latch* passLatch
				) override;

				[[nodiscard]]
				u32 GetJobCount() const noexcept { return static_cast<u32>(m_data.size()); }
			private:
				std::vector<DebugRenderPassData> m_data;

				BufferResourceHandle m_lineVertexBufferHandle;
				PipelineStateResourceHandle m_stateHandle;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_DEBUG_DEBUG_RENDER_PASS_HPP_