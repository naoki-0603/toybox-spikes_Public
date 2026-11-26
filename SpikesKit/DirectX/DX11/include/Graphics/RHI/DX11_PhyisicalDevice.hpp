// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_PHYISICAL_DEVICE_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_PHYISICAL_DEVICE_HPP_

#include "Graphics/RHI/RHI_PhyisicalDevice.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			namespace internal
			{
				static const u32 k_amdVendorId = 0x1002;
				static const u32 k_nvidiaVendorId = 0x10DE;
				static const u32 k_intelVenderId = 0x8086;
				static const u32 k_microsoftVenderId = 0x1414;
			} // namespace internal

			class DX11_PhyisicalDevice final : public graphics::RHI_PhyisicalDevice
			{
			public:
				DX11_PhyisicalDevice();
				DX11_PhyisicalDevice(const DX11_PhyisicalDevice&) = delete;
				DX11_PhyisicalDevice(DX11_PhyisicalDevice&&) noexcept = default;
				virtual ~DX11_PhyisicalDevice() noexcept = default;

				DX11_PhyisicalDevice& operator=(const DX11_PhyisicalDevice&) = delete;
				DX11_PhyisicalDevice& operator=(DX11_PhyisicalDevice&&) noexcept = default;

			public:
				[[nodiscard]]
				bool Create(graphics::PhyisicalDeviceOptions options) override;

				[[nodiscard]]
				bool Destroy() override;

			public:
				[[nodiscard]]
				void* GetNativeAdapter() const override { return m_adapter.Get(); }

				[[nodiscard]]
				u64 QueryCurrentUsage() const override;

			public:
				[[nodiscard]]
				IDXGIFactory6* GetFactory() const { return m_factory.Get(); }

			private:
				[[nodiscard]]
				bool SelectHighPerformanceAdapter() override;

				void Release() override;
			private:
				ComPtr<IDXGIFactory6> m_factory;
				ComPtr<IDXGIAdapter3> m_adapter;
			};


		} // namespace dx11
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_PHYISICAL_DEVICE_HPP_
