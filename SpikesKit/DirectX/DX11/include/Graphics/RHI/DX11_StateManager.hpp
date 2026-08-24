// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_STATE_MANAGER_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_STATE_MANAGER_HPP_

#include "Graphics/RHI/RHI_States.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			namespace internal
			{
				[[nodiscard]]
				D3D11_COMPARISON_FUNC ToD3D11ComparsionFunc(graphics::RHI_ComparsionFunc func);

				[[nodiscard]]
				D3D11_BLEND ToD3D11Blend(graphics::RHI_BlendMode blendMode);

				[[nodiscard]]
				D3D11_BLEND_OP ToD3D11BlendOp(graphics::RHI_BlendOperation blendOp);

				[[nodiscard]]
				UINT ToD3D11ColorWriteEnable(graphics::RHI_ColorWriteMask mask);

				[[nodiscard]]
				D3D11_DEPTH_WRITE_MASK ToD3D11DepthWriteMask(graphics::RHI_DepthWriteMask mask);

				[[nodiscard]]
				D3D11_STENCIL_OP ToD3D11StencilOp(graphics::RHI_StencilOperation stencilOp);

				[[nodiscard]]
				D3D11_CULL_MODE ToD3D11CullMode(graphics::RHI_CullMode cullMode);

				[[nodiscard]]
				D3D11_FILL_MODE ToD3D11FillMode(graphics::RHI_FillMode fillMode);

				[[nodiscard]]
				D3D11_FILTER ToD3D11Filter(graphics::RHI_SamplerFilter filter);

				[[nodiscard]]
				D3D11_TEXTURE_ADDRESS_MODE ToD3D11TextureAddressMode(graphics::RHI_SamplerTextureAddress address);
			} // namespace internal

			class DX11_StateManager final
			{
			public:
				DX11_StateManager();
				DX11_StateManager(const DX11_StateManager&) = delete;
				DX11_StateManager(DX11_StateManager&&) noexcept = default;
				~DX11_StateManager() noexcept = default;

			public:
				[[nodiscard]]
				bool Create(
					ID3D11Device* device
				);

				[[nodiscard]]
				bool Destroy();

			public:
				[[nodiscard]]
				ID3D11BlendState* GetOrCreateBlendState(
					ID3D11Device* device,
					const graphics::RHI_BlendState& blendState
				);

				[[nodiscard]]
				ID3D11DepthStencilState* GetOrCreateDepthStencilState(
					ID3D11Device* device,
					const graphics::RHI_DepthStencilState& depthStencilState
				);

				[[nodiscard]]
				ID3D11RasterizerState* GetOrCreateRasterizerState(
					ID3D11Device* device,
					const graphics::RHI_RasterizerState& rasterizerState
				);

				[[nodiscard]]
				ID3D11SamplerState* GetOrCreateSamplerState(
					ID3D11Device* device,
					const graphics::RHI_SamplerState& samplerState
				);
			private:
				[[nodiscard]]
				bool CreateDefaultBlendStates(ID3D11Device* device);

				[[nodiscard]]
				bool CreateDefaultDepthStencilStates(ID3D11Device* device);

				[[nodiscard]]
				bool CreateDefaultRasterizerStates(ID3D11Device* device);

				[[nodiscard]]
				bool CreateDefaultSamplerStates(ID3D11Device* device);
			private:
				std::unordered_map<u64, ComPtr<ID3D11BlendState>> m_blendStateCache;
				std::unordered_map<u64, ComPtr<ID3D11DepthStencilState>> m_depthStencilStateCache;
				std::unordered_map<u64, ComPtr<ID3D11RasterizerState>> m_rasterizerStateCache;
				std::unordered_map<u64, ComPtr<ID3D11SamplerState>> m_samplerStateCache;
			};
		} // namespace dx11
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_STATE_MANAGER_HPP_