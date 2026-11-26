// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_PIPELINE_STATE_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_PIPELINE_STATE_HPP_

#include "Graphics/RHI/RHI_PipelineState.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			namespace internal
			{
				[[nodiscard]]
				D3D11_CULL_MODE ToD3D11CullMode(graphics::RHI_CullMode cullMode);

				[[nodiscard]]
				D3D11_FILL_MODE ToD3D11FillMode(graphics::RHI_FillMode fillMode);

				[[nodiscard]]
				D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(graphics::RHI_PrimitiveTopology primitiveTopology);
			} // namespace internal

			class DX11_PipelineState final : public graphics::RHI_PipelineState
			{
			public:
				DX11_PipelineState() = delete;
				DX11_PipelineState(
					ID3D11BlendState* blendState,
					ID3D11RasterizerState* rasterizerState,
					ID3D11DepthStencilState* depthStencilState
				);
				DX11_PipelineState(const DX11_PipelineState&) = delete;
				DX11_PipelineState(DX11_PipelineState&&) noexcept = default;
				~DX11_PipelineState() noexcept override = default;

				DX11_PipelineState& operator=(const DX11_PipelineState&) = delete;
				DX11_PipelineState& operator=(DX11_PipelineState&&) noexcept = default;
			public:
				[[nodiscard]]
				bool Create(
					graphics::RHI_Device* device,
					const graphics::RHI_PipelineStateDesc& desc
				) override;

				[[nodiscard]]
				bool Destroy() override;
			public:
				[[nodiscard]]
				ID3D11RasterizerState* GetRasterizerState() const noexcept { return m_rasterizerState; }

				[[nodiscard]]
				ID3D11BlendState* GetBlendState() const noexcept { return m_blendState; }

				[[nodiscard]]
				ID3D11DepthStencilState* GetDepthStencilState() const noexcept { return m_depthStencilState; }

				[[nodiscard]]
				ID3D11InputLayout* GetInputLayout() const noexcept { return m_inputLayout.Get(); }
			private:
				[[nodiscard]]
				bool CreateInputLayout(
					ID3D11Device* device,
					const graphics::RHI_Shader* vertexShader,
					const std::vector<graphics::RHI_VertexLayout>& vertexLayouts
				);
			private:
				void Release() override;

			private:
				ID3D11BlendState* m_blendState;
				ID3D11RasterizerState* m_rasterizerState;
				ID3D11DepthStencilState* m_depthStencilState;
				
				ComPtr<ID3D11InputLayout> m_inputLayout;
			};
		} // namespace dx11
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_PIPELINE_STATE_HPP_