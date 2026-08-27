
// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SWAPCHAIN_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SWAPCHAIN_HPP_

#include "Graphics/RHI/RHI_SwapChain.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			class DX11_Device;
			class DX11_Texture;
			class DX11_TextureView;

			namespace internal
			{
				[[nodiscard]]
				DXGI_SWAP_CHAIN_FLAG ToDXGISwapChainFlag(graphics::RHI_SwapChainOptions options);
			} // namespace internal

			class DX11_SwapChain final : public graphics::RHI_SwapChain
			{
			public:
				DX11_SwapChain();
				DX11_SwapChain(const DX11_SwapChain&) = delete;
				DX11_SwapChain(DX11_SwapChain&&) noexcept = default;
				~DX11_SwapChain() noexcept override = default;

				DX11_SwapChain& operator=(const DX11_SwapChain&) = delete;
				DX11_SwapChain& operator=(DX11_SwapChain&&) noexcept = default;
			public:
				[[nodiscard]]
				bool Create(
					graphics::RHI_Device* device,
					const graphics::RHI_SwapChainDesc& desc
				) override;

				[[nodiscard]]
				bool Destroy() override;
			public:
				[[nodiscard]]
				bool Present() override;

				[[nodiscard]]
				bool Resize(
					const IVector2& size
				) override;
			public:
				[[nodiscard]]
				graphics::RHI_TextureView* GetCurrentRTV() const override { return reinterpret_cast<graphics::RHI_TextureView*>(m_colorTextureView); }

				[[nodiscard]]
				graphics::RHI_TextureView* GetCurrentDSV() const override { return reinterpret_cast<graphics::RHI_TextureView*>(m_depthStencilTextureView); }
			private:
				void Release() override;

			private:
				DX11_Device* m_device;
				
				ComPtr<IDXGISwapChain1> m_swapChain;

				DX11_Texture* m_colorTexture;
				DX11_Texture* m_depthStencilTexture;
				DX11_TextureView* m_colorTextureView;
				DX11_TextureView* m_depthStencilTextureView;
			};
		} // namespace dx11
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SWAPCHAIN_HPP_