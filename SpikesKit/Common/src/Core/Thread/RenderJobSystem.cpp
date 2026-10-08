// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Thread/RenderJobSystem.hpp"

namespace ts
{
	namespace kit
	{
		namespace thread
		{
			RenderJobSystem::RenderJobSystem() :
				m_pushIndex(), m_popIndex(),
				m_jobQueue(), m_isRunning(true)
			{
			}

			RenderJobSystem::~RenderJobSystem() noexcept
			{
			}

			void RenderJobSystem::PushJob(const RenderJob& job)
			{
				u32 index = m_pushIndex.load(std::memory_order_relaxed);

				m_jobQueue[index & k_queueMask] = job;

				m_pushIndex.store(index + 1, std::memory_order_release);
			}

			void RenderJobSystem::Work()
			{
				while (m_isRunning)
				{
					RenderJob job{};

					if (TryPop(job))
					{
						if (job.m_execute)
						{
							job.m_execute(job.m_jobData);
						}

						if (job.m_latch)
						{
							job.m_latch->count_down();
						}
					}
					else
					{
						std::this_thread::yield();
					}
				}
			}

			void RenderJobSystem::OnEngineTerminate(
				const event::EventEngineTerminate* data
			)
			{
				(void)data;

				m_isRunning = false;
			}

			bool RenderJobSystem::TryPop(RenderJob& job)
			{
				// popIndexがpushIndexに追いついたら空っぽ
				const u32 pushIndex = m_pushIndex.load(std::memory_order_acquire);

				while (true)
				{
					u32 popIndex = m_popIndex.load(std::memory_order_relaxed);

					// 仕事なし
					if (popIndex >= pushIndex)
					{
						job = {};

						return false;
					}

					if (m_popIndex.compare_exchange_weak(popIndex, popIndex + 1, std::memory_order_acquire, std::memory_order_relaxed))
					{
						job = std::move(m_jobQueue[popIndex & k_queueMask]);

						break;
					}
				}

				return true;
			}
		} // namespace thread
	} // namespace kit
} // namespace ts