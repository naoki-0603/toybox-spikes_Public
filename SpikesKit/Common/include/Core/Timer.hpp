// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_TIMER_HPP_
#define SPIKES_KIT_COMMON_CORE_TIMER_HPP_

namespace ts
{
	namespace kit
	{
		class Timer final
		{
		public:
			Timer();
			~Timer() noexcept = default;

		public:
			void Begin();
			void End();

		public:
			[[noidiscard]]
			f32 GetDeltaTime() const noexcept { return m_deltaTime; }

			[[nodiscard]]
			f32 GetTime() const noexcept { return m_time; }

			[[nodiscard]]
			u32 GetFPS() const noexcept { return m_fps; }
		private:
			std::chrono::steady_clock::time_point m_beginTime;
			std::chrono::steady_clock::time_point m_endTime;
			
			f32 m_deltaTime;
			f32 m_timer;
			f32 m_time;

			u32 m_fps;
		};
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_TIMER_HPP_