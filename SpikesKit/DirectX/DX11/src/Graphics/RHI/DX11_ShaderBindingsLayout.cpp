// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RHI/DX11_ShaderBindingsLayout.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			DX11_ShaderBindingsLayout::DX11_ShaderBindingsLayout() :
				graphics::RHI_ShaderBindingsLayout()
			{
			}

			bool DX11_ShaderBindingsLayout::Create(
				const graphics::RHI_ShaderBindingsLayoutDesc& desc
			)
			{
				// DX11では設定するだけ
				m_desc = desc;

				return true;
			}

			bool DX11_ShaderBindingsLayout::Destroy()
			{
				Release();

				return true;
			}

			void DX11_ShaderBindingsLayout::Release()
			{
				delete this;
			}
		} // namespace dx11
	} // namespace kit
} // namespace ts