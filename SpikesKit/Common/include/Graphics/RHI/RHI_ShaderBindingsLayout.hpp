// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SHADER_BINDINGS_LAYOUT_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SHADER_BINDINGS_LAYOUT_HPP_

#include "IRHI.hpp"

#include "RHI_Types.hpp"
#include "RHI_States.hpp"
#include "RHI_Sampler.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			constexpr u32 k_maxSamplersSlots = 16u;

			struct RHI_ShaderBindingsLayoutDesc final
			{
				std::array<const RHI_Sampler*, k_maxSamplersSlots> m_psSamplers{};
			};

			class RHI_ShaderBindingsLayout : public IRHI
			{
			public:
				RHI_ShaderBindingsLayout();
				RHI_ShaderBindingsLayout(const RHI_ShaderBindingsLayout&) = delete;
				RHI_ShaderBindingsLayout(RHI_ShaderBindingsLayout&&) noexcept = default;
				virtual ~RHI_ShaderBindingsLayout() noexcept override = default;

				RHI_ShaderBindingsLayout& operator=(const RHI_ShaderBindingsLayout&) = delete;
				RHI_ShaderBindingsLayout& operator=(RHI_ShaderBindingsLayout&&) noexcept = default;

			public:
				[[nodiscard]]
				virtual bool Create(const RHI_ShaderBindingsLayoutDesc& desc) = 0;

			public:
				[[nodiscard]]
				const std::array<const RHI_Sampler*, k_maxSamplersSlots>& GetPSSamplers() const { return m_desc.m_psSamplers; }

			protected:
				RHI_ShaderBindingsLayoutDesc m_desc;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SHADER_BINDINGS_LAYOUT_HPP_