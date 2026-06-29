// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "DX11GUIApplication.hpp"

#include "UnitConversion.hpp"

namespace ts
{
	namespace utils
	{
		namespace dx11
		{
			namespace internal
			{
				bool IsFailure(HRESULT hr)
				{
					return (hr != S_OK);
				}
			}
		}

		DX11GUIApplication::DX11GUIApplication(
			std::string_view name,
			const IVec2& size
		) : Application(name),
			m_size(size), m_window(),
			m_device(), m_deviceContext(), m_debug(),
			m_adapter(), m_swapChain(), m_swapChainRTV(), m_swapChainDSV(),
			m_viewport()
		{
			m_window = std::make_unique<Window>(name, size);
			m_window->SetResizeCallback(
				[this](const IVec2& size) -> void
				{
					this->OnWindowResize(size);
				}
			);
		}

		bool DX11GUIApplication::ShouldClose() const noexcept
		{
			return !(m_window->PollEvents());
		}

		void DX11GUIApplication::OnWindowResize(const IVec2& size)
		{
			m_swapChainRTV.Reset();
			m_swapChainDSV.Reset();

			{
				auto hr = m_swapChain->ResizeBuffers(
					0,
					size.x,
					size.y,
					DXGI_FORMAT_UNKNOWN,
					DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				);

				if (dx11::internal::IsFailure(hr))
				{
					std::println("[DX11GUIApplication::OnWindowResize]: スワップチェインのリサイズに失敗しました。");

					assert(false);
				}
			}
			
			// RTVを再生成する
			{
				ComPtr<ID3D11Texture2D> rtvBuffer;
				auto hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(rtvBuffer.ReleaseAndGetAddressOf()));
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11GUIApplication::OnWindowResize]: スワップチェインのバッファの取得に失敗しました。");
				}

				D3D11_TEXTURE2D_DESC texture2dDesc = {};
				rtvBuffer->GetDesc(&texture2dDesc);

				D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				rtvDesc.Format = texture2dDesc.Format;
				rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;

				hr = m_device->CreateRenderTargetView(
					rtvBuffer.Get(),
					&rtvDesc,
					m_swapChainRTV.ReleaseAndGetAddressOf()
				);
				if (dx11::internal::IsFailure(hr))
				{
					assert(false);
				}
			}

			// DSVを再生成する
			{
				ComPtr<ID3D11Texture2D> depthStencilBuffer;
				D3D11_TEXTURE2D_DESC texture2DDesc = {};
				texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				texture2DDesc.Width = size.x;
				texture2DDesc.Height = size.y;
				texture2DDesc.ArraySize = 1;
				texture2DDesc.MipLevels = 1;
				texture2DDesc.CPUAccessFlags = 0;
				texture2DDesc.SampleDesc.Count = 1;
				texture2DDesc.SampleDesc.Quality = 0;
				texture2DDesc.MiscFlags = 0;
				texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
				texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

				auto hr = m_device->CreateTexture2D(
					&texture2DDesc,
					nullptr,
					depthStencilBuffer.ReleaseAndGetAddressOf()
				);
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11GUIApplication::OnWindowResize]: デプスバッファテクスチャの生成に失敗しました。");
				}

				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
				depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Texture2D.MipSlice = 0;

				hr = m_device->CreateDepthStencilView(
					depthStencilBuffer.Get(),
					&depthStencilViewDesc,
					m_swapChainDSV.ReleaseAndGetAddressOf()
				);
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11GUIApplication::OnWindowResize]: デプスステンシルビューの作成に失敗しました。");

					assert(false);
				}
			}

			m_viewport.Width = size.x;
			m_viewport.Height = size.y;
		}

		bool DX11GUIApplication::OnInitialize()
		{
			if (!InitializeDirectX11())
			{
				return false;
			}

			if (!InitializeImGui())
			{
				return false;
			}

			return true;
		}

		void DX11GUIApplication::Tick()
		{
			// Update
			{

			}

			// Render
			{
				BeginRender();

				Render();

				EndRender();

				Present();
			}
		}

		void DX11GUIApplication::BeginRender()
		{
			// Bind
			{
				ID3D11RenderTargetView* rtvs[] =
				{
					m_swapChainRTV.Get()
				};

				m_deviceContext->OMSetRenderTargets(
					1u,
					rtvs,
					m_swapChainDSV.Get()
				);

				m_deviceContext->RSSetViewports(
					1u,
					&m_viewport
				);
			}

			// Clear RTV and DSV.
			{
				m_deviceContext->ClearRenderTargetView(
					m_swapChainRTV.Get(),
					dx11::internal::k_defaultClearColor
				);

				m_deviceContext->ClearDepthStencilView(
					m_swapChainDSV.Get(),
					D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
					1.0f, 0u
				);
			}

			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
			}
		}

		void DX11GUIApplication::Render()
		{
			if (ImGui::Begin("MainWindow##DX11GUIApplication"))
			{
				ImGui::Text("Hello DX11 GUI Application!");

			}
			ImGui::End();
		}

		void DX11GUIApplication::EndRender()
		{
			ImGui::Render();

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		void DX11GUIApplication::Present()
		{
			auto hr = m_swapChain->Present(1, 0);
			if (dx11::internal::IsFailure(hr))
			{
				std::println("[DX11]: バックバッファの切り替えに失敗しました。");
				assert(false);
			}
		}

		bool DX11GUIApplication::OnFinalize()
		{
			if (!FinalizeDirectX11())
			{
				return false;
			}

			if (!FinalizeImGui())
			{
				return false;
			}

			return true;
		}

		bool DX11GUIApplication::InitializeDirectX11()
		{
			u32 createDeviceFlags = 0u;
			u32 createFactoryFlags = 0u;

#if TS_DEBUG || TS_RELEASE
			createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // TS_DEBUG || TS_RELEASE

			// ファクトリーを生成する
			ComPtr<IDXGIFactory6> factory;
			{
				auto hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));
				if (dx11::internal::IsFailure(hr))
				{
					std::println("[DX11]: ファクトリーの生成に失敗しました。");

					return false;
				}

				// アダプターを選択する
				if (!SelectHighPerformanceAdapter(factory))
				{
					std::println("[DX11]: アダプターの選択に失敗しました。");

					return false;
				}
			}

			// デバイス/デバイスコンテキストを作成する
			{
				const D3D_FEATURE_LEVEL featureLevels[2] =
				{
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_1
				};

				auto hr = D3D11CreateDevice(
					m_adapter.Get(),
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					createDeviceFlags,
					featureLevels,
					sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
					D3D11_SDK_VERSION,
					m_device.ReleaseAndGetAddressOf(),
					nullptr,
					m_deviceContext.ReleaseAndGetAddressOf()
				);
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11]: デバイスの生成に失敗しました。");

					return false;
				}
			}

			// スワップチェインを生成する
			{
				const HWND handle = m_window->GetHandle();

				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = m_window->GetSize().x;
				swapChainDesc.Height = m_window->GetSize().y;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = FALSE;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = 2;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
				swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

				auto hr = factory->CreateSwapChainForHwnd(
					m_device.Get(),
					handle,
					&swapChainDesc,
					nullptr,
					nullptr,
					m_swapChain.ReleaseAndGetAddressOf()
				);
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11]: スワップチェインの生成に失敗しました。");

					return false;
				}

				// ALT+ENTERを無効化する
				hr = factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
				if (dx11::internal::IsFailure(hr))
				{
					std::print("[DX11]: ALT+ENTERの無効化に失敗しました。");

					return false;
				}

				// カラーバッファのRTVを作成する
				{
					ComPtr<ID3D11Texture2D> rtvBuffer;
					hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(rtvBuffer.ReleaseAndGetAddressOf()));
					if (dx11::internal::IsFailure(hr))
					{
						std::print("[DX11]: スワップチェインのバッファの取得に失敗しました。");

						return false;
					}

					D3D11_TEXTURE2D_DESC texture2dDesc = {};
					rtvBuffer->GetDesc(&texture2dDesc);

					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = texture2dDesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					rtvDesc.Texture2D.MipSlice = 0;

					hr = m_device->CreateRenderTargetView(
						rtvBuffer.Get(),
						&rtvDesc,
						m_swapChainRTV.ReleaseAndGetAddressOf()
					);
					if (dx11::internal::IsFailure(hr))
					{
						return false;
					}
				}

				// デプスバッファを作成する
				{
					ComPtr<ID3D11Texture2D> depthStencilBuffer;
					D3D11_TEXTURE2D_DESC texture2DDesc = {};
					texture2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					texture2DDesc.Width = m_window->GetSize().x;
					texture2DDesc.Height = m_window->GetSize().y;
					texture2DDesc.ArraySize = 1;
					texture2DDesc.MipLevels = 1;
					texture2DDesc.CPUAccessFlags = 0;
					texture2DDesc.SampleDesc.Count = 1;
					texture2DDesc.SampleDesc.Quality = 0;
					texture2DDesc.MiscFlags = 0;
					texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
					texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

					hr = m_device->CreateTexture2D(
						&texture2DDesc,
						nullptr,
						depthStencilBuffer.ReleaseAndGetAddressOf()
					);
					if (dx11::internal::IsFailure(hr))
					{
						std::print("[DX11]: デプスバッファテクスチャの生成に失敗しました。");

						return false;
					}

					D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
					depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					depthStencilViewDesc.Texture2D.MipSlice = 0;

					hr = m_device->CreateDepthStencilView(
						depthStencilBuffer.Get(),
						&depthStencilViewDesc,
						m_swapChainDSV.ReleaseAndGetAddressOf()
					);
					if (dx11::internal::IsFailure(hr))
					{
						std::print("[DX11]: デプスステンシルビューの作成に失敗しました。");

						return false;
					}
				}
			}

			// ビューポートの作成
			{
				m_viewport.TopLeftX = 0;
				m_viewport.TopLeftY = 0;
				m_viewport.Width = static_cast<FLOAT>(m_window->GetSize().x);
				m_viewport.Height = static_cast<FLOAT>(m_window->GetSize().y);
				m_viewport.MinDepth = 0.0f;
				m_viewport.MaxDepth = 1.0f;
			}


			return true;
		}

		bool DX11GUIApplication::InitializeImGui()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			ImGui::StyleColorsDark();

			if (!ImGui_ImplWin32_Init(m_window->GetHandle()))
			{
				std::print("[ImGui]: ImGui_ImplWin32_Init の初期化に失敗しました。");

				return false;
			}
			
			if (!ImGui_ImplDX11_Init(m_device.Get(), m_deviceContext.Get()))
			{
				std::print("[ImGui]: ImGui_ImplDX11_Init の初期化に失敗しました。");

				return false;
			}

			return true;
		}

		bool DX11GUIApplication::SelectHighPerformanceAdapter(
			const ComPtr<IDXGIFactory6>& factory
		)
		{
			auto hr = S_OK;

			ComPtr<IDXGIAdapter3> enumeratedAdapter;
			for (u32 adapterIndex = 0u; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(
				adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf())); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc = {};
				hr = enumeratedAdapter->GetDesc1(&desc);
				if (dx11::internal::IsFailure(hr))
				{
					std::println("[DX11]: アダプターのDESC1構造体の取得に失敗しました。");

					return false;
				}

				if (desc.VendorId == dx11::internal::k_amdVendorId ||
					desc.VendorId == dx11::internal::k_nvidiaVendorId)
				{
					std::println("DedicatedVideoMemory: {}MB", ToMB(ToKB(desc.DedicatedVideoMemory)));
					std::println("DedicatedSystemMemory: {}MB", ToMB(ToKB(desc.DedicatedSystemMemory)));
					std::println("SharedSystemMemory: {}MB", ToMB(ToKB(desc.SharedSystemMemory)));

					break;
				}
			}

			m_adapter.Attach(enumeratedAdapter.Detach());

			return true;
		}
		
		bool DX11GUIApplication::FinalizeDirectX11()
		{
			return true;
		}

		bool DX11GUIApplication::FinalizeImGui()
		{
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();

			ImGui::DestroyContext();

			return true;
		}
	} // namespace utils
} // namespace ts