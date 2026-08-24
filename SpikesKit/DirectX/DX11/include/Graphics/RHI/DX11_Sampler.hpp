// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SAMPLER_HPP_
#define SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SAMPLER_HPP_

#include "Graphics/RHI/RHI_Sampler.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			class DX11_Sampler final : public graphics::RHI_Sampler
			{
			public:
				DX11_Sampler(ID3D11SamplerState* sampler);
				DX11_Sampler(const DX11_Sampler&) = delete;
				DX11_Sampler(DX11_Sampler&&) noexcept = default;
				virtual ~DX11_Sampler() noexcept override = default;

				DX11_Sampler& operator=(const DX11_Sampler&) = delete;
				DX11_Sampler& operator=(DX11_Sampler&&) noexcept = default;
			public:
				[[nodiscard]]
				bool Create(
					graphics::RHI_Device* device,
					const graphics::RHI_SamplerState& state
				) override;

				[[nodiscard]]
				bool Destroy() override;
			public:
				[[nodiscard]]
				void* GetNativeSampler() const override { return m_sampler; }

			private:
				void Release() override;

			private:
				ID3D11SamplerState* m_sampler;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_GRAPHICS_RHI_DX11_SAMPLER_HPP_