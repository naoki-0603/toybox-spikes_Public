// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_PhyisicalDevice.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_PhyisicalDevice::DX11_PhyisicalDevice() : RHI_PhyisicalDevice(),
				m_factory(), m_adapter()
			{
			}

			bool DX11_PhyisicalDevice::Create(graphics::PhyisicalDeviceOptions options)
			{
				u32 createFactoryFlags = 0u;
				{
					switch (options)
					{
						case graphics::PhyisicalDeviceOptions::EnableDebug:
						{
							createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
						}
						break;
					}
				}
				
				{
					HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(m_factory.ReleaseAndGetAddressOf()));
					if (!(hr == S_OK))
					{
						TS_FATAL_LOG("ファクトリーの生成に失敗しました。");
						
						return false;
					}

					if (!SelectHighPerformanceAdapter())
					{
						TS_FATAL_LOG("アダプターの選択に失敗しました。");

						return false;
					}
				}

				return true;
			}

			bool DX11_PhyisicalDevice::Destroy()
			{
				Release();

				return true;
			}

			u64 DX11_PhyisicalDevice::QueryCurrentUsage() const
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo = {};
				HRESULT hr = m_adapter->QueryVideoMemoryInfo(
					0u,
					DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
					&memoryInfo
				);
				if (hr != S_OK)
				{
					TS_ERROR_LOG("VRAM使用量の取得に失敗しました。");

					return UINT64_MAX;
				}

				return FormatBytes(memoryInfo.CurrentUsage);
			}

			bool DX11_PhyisicalDevice::SelectHighPerformanceAdapter()
			{
				HRESULT hr = S_OK;

				ComPtr<IDXGIAdapter3> enumeratedAdapter;
				for (u32 adapterIndex = 0u; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapterByGpuPreference(
					adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf())); ++adapterIndex)
				{
					DXGI_ADAPTER_DESC1 desc = {};
					hr = enumeratedAdapter->GetDesc1(&desc);
					if (!(hr == S_OK))
					{
						TS_FATAL_LOG("アダプターのDESC1構造体の取得に失敗しました。");

						return false;
					}

					if (desc.VendorId == dx11::internal::k_amdVendorId ||
						desc.VendorId == dx11::internal::k_nvidiaVendorId ||
						desc.VendorId == dx11::internal::k_intelVenderId ||
						desc.VendorId == dx11::internal::k_microsoftVenderId)
					{
						TS_INFO_LOG("DedicatedVideoMemory: {}MB", ToMB(ToKB(desc.DedicatedVideoMemory)));
						TS_INFO_LOG("DedicatedSystemMemory: {}MB", ToMB(ToKB(desc.DedicatedSystemMemory)));
						TS_INFO_LOG("SharedSystemMemory: {}MB", ToMB(ToKB(desc.SharedSystemMemory)));

						break;
					}
				}

				m_adapter.Attach(enumeratedAdapter.Detach());
				if (!m_adapter)
				{
					return false;
				}

				return true;
			}

			void DX11_PhyisicalDevice::Release()
			{
				m_factory.Reset();
				m_adapter.Reset();

				// DLL側で寿命を管理するため
				delete this;
			}

			extern "C" __declspec(dllexport) graphics::RHI_PhyisicalDevice* CreateRHIPhyisicalDevice()
			{
				return new DX11_PhyisicalDevice;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts