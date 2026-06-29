// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Timer.hpp"

namespace ts
{
	namespace kit
	{
		Timer::Timer() :
			m_beginTime(), m_endTime(),
			m_deltaTime(), m_timer(), 
			m_time(), m_fps()
		{
		}

		void Timer::Begin()
		{
			m_beginTime = std::chrono::steady_clock::now();
		}
		
		void Timer::End()
		{
			m_endTime = std::chrono::steady_clock::now();

			std::chrono::duration<f32> timeSpan = m_endTime - m_beginTime;
			m_deltaTime = timeSpan.count();

			m_timer += m_deltaTime;
			m_time += m_deltaTime;

			if (m_timer >= 1.0f)
			{
				if (m_deltaTime > 0.0f)
				{
					m_fps = static_cast<u32>(1.0f / m_deltaTime);
				}
				else
				{
					m_fps = 0;
				}

				m_timer = 0.0f;
			}
		}
	} // namespace kit
} // namespace ts