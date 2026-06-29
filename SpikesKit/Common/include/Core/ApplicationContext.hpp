// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_APPLICATION_CONTEXT_HPP_
#define SPIKES_KIT_COMMON_CORE_APPLICATION_CONTEXT_HPP_

namespace ts
{
	namespace kit
	{
		class Timer;
		class InputSystem;
		class Window;

		struct ApplicationContext
		{
			ApplicationContext() = delete;
			ApplicationContext(
				Timer& timerRef,
				InputSystem& inputSystemRef,
				Window& windowRef
			);

			Timer& m_timerRef;
			InputSystem& m_inputSystemRef;
			Window& m_windowRef;
		};
	} // namespace kit
} // namespace ts

#endif // !SPIKES_KIT_COMMON_CORE_APPLICATION_CONTEXT_HPP_