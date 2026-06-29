// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_UTILS_DX11_GUI_APPLICATION_HPP_
#define SPIKES_UTILS_DX11_GUI_APPLICATION_HPP_

#include "Application.hpp"
#include "Window.hpp"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"


namespace ts
{
	namespace utils
	{
		namespace dx11::internal
		{
			[[nodiscard]]
			bool IsFailure(HRESULT hr);

			static const u32 k_amdVendorId = 0x1002;
			static const u32 k_nvidiaVendorId = 0x10DE;
			static const float k_defaultClearColor[4] = { 0.67f, 0.88f, 0.98f, 1.0f };
		}

		class DX11GUIApplication : public Application
		{
		public:
			DX11GUIApplication(std::string_view name, const IVec2& size = IVec2(640, 480));
			virtual ~DX11GUIApplication() noexcept override = default;

			virtual bool ShouldClose() const noexcept;

			void OnWindowResize(const IVec2& size);
		protected:
			virtual bool OnInitialize() override;
			virtual void Tick() override;

			virtual void BeginRender();
			virtual void Render();
			virtual void EndRender();
			virtual void Present();

			virtual bool OnFinalize() override;

		private:
			bool InitializeDirectX11();
			bool InitializeImGui();
			bool SelectHighPerformanceAdapter(const ComPtr<IDXGIFactory6>& factory);

			bool FinalizeDirectX11();
			bool FinalizeImGui();

		private:
			IVec2 m_size;
			
			std::unique_ptr<Window> m_window;

			// DX11 Resources.
			ComPtr<ID3D11Device> m_device;
			ComPtr<ID3D11DeviceContext> m_deviceContext;
			ComPtr<ID3D11Debug> m_debug;
			ComPtr<IDXGIAdapter> m_adapter;
			ComPtr<IDXGISwapChain1> m_swapChain;
			ComPtr<ID3D11RenderTargetView> m_swapChainRTV;
			ComPtr<ID3D11DepthStencilView> m_swapChainDSV;

			D3D11_VIEWPORT m_viewport;

		};
	} // namespace utils
} // namespace ts

#endif // !SPIKES_UTILS_DX11_GUI_APPLICATION_HPP_