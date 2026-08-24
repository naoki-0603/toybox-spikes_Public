// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_Sampler.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_Sampler::DX11_Sampler(ID3D11SamplerState* sampler) :
				RHI_Sampler(), m_sampler(sampler)
			{
			}
			
			bool DX11_Sampler::Create(
				graphics::RHI_Device* device,
				const graphics::RHI_SamplerState& state
			)
			{
				m_samplerState = state;

				return true;
			}
			
			bool DX11_Sampler::Destroy()
			{
				Release();

				return true;
			}

			void DX11_Sampler::Release()
			{
				delete this;
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts