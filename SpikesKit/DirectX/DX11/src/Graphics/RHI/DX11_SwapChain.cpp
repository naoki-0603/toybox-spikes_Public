// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_SwapChain.hpp"
#include "Graphics/RHI/DX11_Device.hpp"
#include "Graphics/RHI/DX11_PhyisicalDevice.hpp"
#include "Graphics/RHI/DX11_Format.hpp"
#include "Graphics/RHI/DX11_Texture.hpp"
#include "Graphics/RHI/DX11_TextureView.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			namespace internal
			{
				[[nodiscard]]
				DXGI_SWAP_CHAIN_FLAG ToDXGISwapChainFlag(graphics::RHI_SwapChainOptions options)
				{
					DXGI_SWAP_CHAIN_FLAG flags = {};
					if ((options & graphics::RHI_SwapChainOptions::AllowTearing) == graphics::RHI_SwapChainOptions::AllowTearing)
					{
						flags = static_cast<DXGI_SWAP_CHAIN_FLAG>(flags | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
					}

					return flags;
				}
			} // namespace internal

			DX11_SwapChain::DX11_SwapChain() : RHI_SwapChain(),
				m_device(), m_swapChain(),
				m_colorTexture(), m_depthStencilTexture(),
				m_colorTextureView(), m_depthStencilTextureView()
			{
			}
			
			bool DX11_SwapChain::Create(
				graphics::RHI_Device* device,
				const graphics::RHI_SwapChainDesc& desc
			)
			{
				TS_ASSERT(desc.m_windowHandle, "WindowHandleがnullです");


				auto dx11Device = static_cast<DX11_Device*>(device);
				auto phyisicalDevice = static_cast<DX11_PhyisicalDevice*>(device->GetPhysicalDevice());
				auto hwnd = static_cast<HWND>(desc.m_windowHandle);

				IDXGIFactory6* factory = phyisicalDevice->GetFactory();
				
				// スワップチェインの作成
				{
					DXGI_SWAP_CHAIN_DESC1 dxgiDesc = {};
					dxgiDesc.Width = static_cast<u32>(desc.m_size.x);
					dxgiDesc.Height = static_cast<u32>(desc.m_size.y);
					dxgiDesc.Format = ToDXGIFormat(desc.m_format);
					dxgiDesc.BufferCount = desc.m_bufferCount;
					dxgiDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
					dxgiDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
					dxgiDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
					dxgiDesc.Scaling = DXGI_SCALING_STRETCH;
					dxgiDesc.Stereo = static_cast<BOOL>(false);
					dxgiDesc.Flags = internal::ToDXGISwapChainFlag(desc.m_options);
					dxgiDesc.SampleDesc.Count = 1u;
					dxgiDesc.SampleDesc.Quality = 0u;

					const HRESULT hr = factory->CreateSwapChainForHwnd(
						dx11Device->GetNativeDevice(),
						hwnd,
						&dxgiDesc,
						nullptr,
						nullptr,
						m_swapChain.ReleaseAndGetAddressOf()
					);
					if (hr != S_OK)
					{
						TS_FATAL_LOG("SwapChainの作成に失敗しました。");

						return false;
					}
				}

				// バックバッファのビューを作成する
				{
					ComPtr<ID3D11Resource> rtvBuffer;
					const HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(rtvBuffer.ReleaseAndGetAddressOf()));
					if (hr != S_OK)
					{
						TS_FATAL_LOG("スワップチェインのバッファの取得に失敗しました。");

						return false;
					}

					// RTV
					{
						m_colorTexture = new DX11_Texture(rtvBuffer);
						
						graphics::RHI_TextureViewDesc viewDesc = {};
						viewDesc.m_texture = m_colorTexture;
						viewDesc.m_type = graphics::RHI_TextureViewType::RenderTargetView;

						m_colorTextureView = static_cast<DX11_TextureView*>(dx11Device->CreateTextureView(viewDesc));
						if (!m_colorTextureView)
						{
							TS_FATAL_LOG("ColorTextureViewの作成に失敗しました。");

							return false;
						}
					}

					// DSV
					{
						graphics::RHI_TextureDesc textureDesc = {};
						textureDesc.m_format = graphics::RHI_Format::D24_UNorm_S8_UInt;
						textureDesc.m_size = desc.m_size;
						textureDesc.m_mipLevels = 1u;
						textureDesc.m_type = graphics::RHI_TextureType::Texture2D;
						textureDesc.m_usage = graphics::RHI_TextureUsage::DepthStencil;

						m_depthStencilTexture = static_cast<DX11_Texture*>(dx11Device->CreateTexture(textureDesc));
						if (!m_depthStencilTexture)
						{
							TS_FATAL_LOG("DepthStencilTextureの作成に失敗しました。");

							return false;
						}

						graphics::RHI_TextureViewDesc textureViewDesc = {};
						textureViewDesc.m_texture = m_depthStencilTexture;
						textureViewDesc.m_type = graphics::RHI_TextureViewType::DepthStencilView;

						m_depthStencilTextureView = static_cast<DX11_TextureView*>(dx11Device->CreateTextureView(textureViewDesc));
						if (!m_depthStencilTextureView)
						{
							TS_FATAL_LOG("DepthStencilTextureViewの作成に失敗しました。");

							return false;
						}
					}
				}

				// Viewport
				{
					m_viewport.m_topLeft = { };
					m_viewport.m_size = desc.m_size;
					m_viewport.m_minDepth = 0.0f;
					m_viewport.m_maxDepth = 1.0f;
				}

				m_options = desc.m_options;
				m_device = dx11Device;

				return true;
			}
			
			bool DX11_SwapChain::Destroy()
			{
				if (!m_colorTexture->Destroy()) return false;
				if (!m_colorTextureView->Destroy()) return false;
				if (!m_depthStencilTexture->Destroy()) return false;
				if (!m_depthStencilTextureView->Destroy()) return false;

				Release();

				return true;
			}
			
			bool DX11_SwapChain::Present()
			{
				const HRESULT hr = m_swapChain->Present(
					0u,
					internal::ToDXGISwapChainFlag(m_options)
				);

				return (hr == S_OK);
			}
			
			bool DX11_SwapChain::Resize(const IVector2& size)
			{
				// 既存のテクスチャ/ビューを破棄する
				{
					if (!m_colorTexture->Destroy())
					{
						TS_FATAL_LOG("ColorTextureの破棄に失敗しました。");

						return false;
					}

					if (!m_depthStencilTexture->Destroy())
					{
						TS_FATAL_LOG("DepthStencilTextureの破棄に失敗しました。");

						return false;
					}

					if (!m_colorTextureView->Destroy())
					{
						TS_FATAL_LOG("ColorTextureViewの破棄に失敗しました。");

						return false;
					}

					if (!m_depthStencilTextureView->Destroy())
					{
						TS_FATAL_LOG("DepthStencilTextureViewの破棄に失敗しました。");

						return false;
					}
				}

				// バッファのリサイズ
				HRESULT hr = m_swapChain->ResizeBuffers(
					0,
					size.x,
					size.y,
					DXGI_FORMAT_UNKNOWN,
					internal::ToDXGISwapChainFlag(m_options)
				);

				if (hr != S_OK)
				{
					TS_FATAL_LOG("[スワップチェインのリサイズに失敗しました。");

					return false;
				}

				// バックバッファを取得する
				ComPtr<ID3D11Resource> rtvBuffer;
				hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(rtvBuffer.ReleaseAndGetAddressOf()));
				if (hr != S_OK)
				{
					TS_FATAL_LOG("スワップチェインのバッファの取得に失敗しました。");

					return false;
				}

				// 新たに生成
				{
					// RTV
					{
						m_colorTexture = new DX11_Texture(rtvBuffer);

						graphics::RHI_TextureViewDesc viewDesc = {};
						viewDesc.m_texture = m_colorTexture;
						viewDesc.m_type = graphics::RHI_TextureViewType::RenderTargetView;

						m_colorTextureView = static_cast<DX11_TextureView*>(m_device->CreateTextureView(viewDesc));
						if (!m_colorTextureView)
						{
							TS_FATAL_LOG("ColorTextureViewの作成に失敗しました。");

							return false;
						}
					}

					// DSV
					{
						graphics::RHI_TextureDesc textureDesc = {};
						textureDesc.m_format = graphics::RHI_Format::D24_UNorm_S8_UInt;
						textureDesc.m_size = size;
						textureDesc.m_mipLevels = 1u;
						textureDesc.m_type = graphics::RHI_TextureType::Texture2D;
						textureDesc.m_usage = graphics::RHI_TextureUsage::DepthStencil;

						m_depthStencilTexture = static_cast<DX11_Texture*>(m_device->CreateTexture(textureDesc));
						if (!m_depthStencilTexture)
						{
							TS_FATAL_LOG("DepthStencilTextureの作成に失敗しました。");

							return false;
						}

						graphics::RHI_TextureViewDesc textureViewDesc = {};
						textureViewDesc.m_texture = m_depthStencilTexture;
						textureViewDesc.m_type = graphics::RHI_TextureViewType::DepthStencilView;

						m_depthStencilTextureView = static_cast<DX11_TextureView*>(m_device->CreateTextureView(textureViewDesc));
						if (!m_depthStencilTextureView)
						{
							TS_FATAL_LOG("DepthStencilTextureViewの作成に失敗しました。");

							return false;
						}
					}
				}

				m_viewport.m_size = size;
				
				return true;
			}

			void DX11_SwapChain::Release()
			{
				m_swapChain.Reset();

				delete this;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts
