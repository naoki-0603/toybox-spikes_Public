// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_BUILDER_DEFAULT_RENDER_RESOURCES_BUILDER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_BUILDER_DEFAULT_RENDER_RESOURCES_BUILDER_HPP_

namespace ts
{
	namespace kit
	{
		namespace resource
		{
			class RenderResourceManager;
		}

		namespace graphics
		{
			struct DefaultRenderResourcesBuilderDesc final
			{
				resource::RenderResourceManager* m_renderResourceManager{};
				ShaderResourceHandle m_staticMeshVertexShaderHandle{};
				ShaderResourceHandle m_meshPBRPixelShaderHandle{};
				ShaderResourceHandle m_meshPBRWithMaskPixelShaderHandle{};
			};

			class DefaultRenderResourcesBuilder final
			{
			public:
				DefaultRenderResourcesBuilder();
				DefaultRenderResourcesBuilder(const DefaultRenderResourcesBuilder&) = delete;
				DefaultRenderResourcesBuilder(DefaultRenderResourcesBuilder&&) noexcept = default;
				~DefaultRenderResourcesBuilder() noexcept = default;

			public:
				[[nodiscard]]
				bool Build(const DefaultRenderResourcesBuilderDesc& desc);

			public:
				[[nodiscard]]
				PipelineStateResourceHandle GetOpaquePipelineStateHandle() const { return m_opaquePipelineStateHandle; }

				[[nodiscard]]
				PipelineStateResourceHandle GetMaskPipelineStateHandle() const { return m_maskPipelineStateHandle; }

				[[nodiscard]]
				PipelineStateResourceHandle GetTransparentPipelineStateHandle() const { return m_transparentPipelineStateHandle; }
			private:
				SamplerResourceHandle m_pointSamplerHandle;
				SamplerResourceHandle m_linearSamplerHandle;
				SamplerResourceHandle m_anisotropicSamplerHandle;

				ShaderBindingsLayoutResourceHandle m_meshShaderBindingsLayoutHandle;

				PipelineStateResourceHandle m_opaquePipelineStateHandle;
				PipelineStateResourceHandle m_maskPipelineStateHandle;
				PipelineStateResourceHandle m_transparentPipelineStateHandle;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_BUILDER_DEFAULT_RENDER_RESOURCES_BUILDER_HPP_