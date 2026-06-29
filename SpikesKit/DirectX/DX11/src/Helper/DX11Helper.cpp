// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Helper/DX11Helper.hpp"

namespace ts
{
	namespace kit
	{
		namespace dx11
		{
			bool IsSucceeded(HRESULT hr) noexcept
			{
				return (hr == S_OK);
			}
		} // namespace dx11
 	} // namespace kit
} // namespace ts