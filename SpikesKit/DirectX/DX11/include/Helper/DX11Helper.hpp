// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_DX11_HELPER_DX11_HELPER_HPP_
#define SPIKES_KIT_DX11_HELPER_DX11_HELPER_HPP_

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			[[nodiscard]]
			bool IsSucceeded(HRESULT hr) noexcept;
		}
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_DX11_HELPER_DX11_HELPER_HPP_