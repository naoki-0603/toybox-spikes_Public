// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/ApplicationContext.hpp"

namespace ts
{
	namespace kit
	{
		ApplicationContext::ApplicationContext(
			Timer& timerRef,
			InputSystem& inputSystemRef,
			Window& windowRef
		) : m_timerRef(timerRef), 
			m_inputSystemRef(inputSystemRef),
			m_windowRef(windowRef)
		{
		}
	} // namespace kit
} // namespace ts
