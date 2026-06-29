// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_CONTEXT_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_CONTEXT_HPP_

namespace ts
{
	namespace kit
	{
		class RHI_Device;
		class RHI_SwapChain;
		
		class RHI_Context
		{
		public:
			RHI_Context();
			RHI_Context(const RHI_Context&) = delete;
			RHI_Context(RHI_Context&&) noexcept = default;
			virtual ~RHI_Context() noexcept = default;

			RHI_Context& operator=(const RHI_Context&) = delete;
			RHI_Context& operator=(RHI_Context&&) noexcept = default;

		public:
			[[nodiscard]]
			virtual bool Create() = 0;

			[[nodiscard]]
			virtual bool Destroy() = 0;
			
			[[nodiscard]]
			const Unique<RHI_Device>& GetDevice() const { return m_device; }

			[[nodiscard]]
			const Unique<RHI_SwapChain>& GetSwapChain() const { return m_swapChain; }
		private:
			Unique<RHI_Device> m_device;
			Unique<RHI_SwapChain> m_swapChain;
		};
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_CONTEXT_HPP_