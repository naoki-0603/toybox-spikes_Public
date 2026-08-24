// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RENDER_PASS_MASK_RENDER_PASS_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RENDER_PASS_MASK_RENDER_PASS_HPP_

#include "RenderPass.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct MaskRenderPassData final
			{
				RenderCommandRecorder* m_recorder{};
				RenderResourceRecorder* m_resourceRecorder{};
				const resource::RenderResourceManager* m_renderResourceManager{};
				const std::vector<RenderPacket>* m_renderPackets{};

				PipelineStateResourceHandle m_pipelineStateHandle{};
				BufferResourceHandle m_sceneConstantsBufferHandle{};
				BufferResourceHandle m_objectConstantsBufferHandle{};
			};

			class MaskRenderPass final : public RenderPass
			{
			public:
				MaskRenderPass(
					PipelineStateResourceHandle pipelineStateHandle
				);
				MaskRenderPass(const MaskRenderPass&) = delete;
				MaskRenderPass(MaskRenderPass&&) noexcept = default;
				~MaskRenderPass() noexcept override = default;
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
				std::vector<MaskRenderPassData> m_data;

				PipelineStateResourceHandle m_pipelineStateHandle;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RENDER_PASS_MASK_RENDER_PASS_HPP_