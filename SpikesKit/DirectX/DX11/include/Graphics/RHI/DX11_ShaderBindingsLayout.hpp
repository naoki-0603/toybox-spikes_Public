// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SHADER_BINDINGS_LAYOUT_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SHADER_BINDINGS_LAYOUT_HPP_

#include "Graphics/RHI/RHI_ShaderBindingsLayout.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			class DX11_ShaderBindingsLayout final : public graphics::RHI_ShaderBindingsLayout
			{
			public:
				DX11_ShaderBindingsLayout();
				DX11_ShaderBindingsLayout(const DX11_ShaderBindingsLayout&) = delete;
				DX11_ShaderBindingsLayout(DX11_ShaderBindingsLayout&&) noexcept = default;
				~DX11_ShaderBindingsLayout() noexcept override = default;

				DX11_ShaderBindingsLayout& operator=(const DX11_ShaderBindingsLayout&) = delete;
				DX11_ShaderBindingsLayout& operator=(DX11_ShaderBindingsLayout&&) noexcept = default;
			public:
				[[nodiscard]]
				bool Create(const graphics::RHI_ShaderBindingsLayoutDesc& desc) override;

				[[nodiscard]]
				bool Destroy() override;
			private:
				void Release() override;
			};
		} // namespace dx11
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SHADER_BINDINGS_LAYOUT_HPP_