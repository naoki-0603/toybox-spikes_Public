// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SAMPLER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SAMPLER_HPP_

#include "IRHI.hpp"

#include "RHI_States.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class RHI_Device;

			class RHI_Sampler : public IRHI
			{
			public:
				RHI_Sampler();
				RHI_Sampler(const RHI_Sampler&) = delete;
				RHI_Sampler(RHI_Sampler&&) noexcept = default;
				virtual ~RHI_Sampler() noexcept override = default;

				RHI_Sampler& operator=(const RHI_Sampler&) = delete;
				RHI_Sampler& operator=(RHI_Sampler&&) noexcept = default;
			public:
				[[nodiscard]]
				virtual bool Create(
					RHI_Device* device,
					const RHI_SamplerState& state
				) = 0;

			public:
				[[nodiscard]]
				const RHI_SamplerState& GetSamplerState() const { return m_samplerState; }

				[[nodiscard]]
				virtual void* GetNativeSampler() const = 0;
			protected:
				RHI_SamplerState m_samplerState;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RHI_RHI_SAMPLER_HPP_