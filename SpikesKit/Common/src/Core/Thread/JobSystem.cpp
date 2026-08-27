// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Thread/JobSystem.hpp"

namespace ts
{
	namespace kit
	{
		namespace thread
		{
			void JobControlBlock::Reset()
			{
				m_dependents.clear();
				m_unresolvedDependecies.store(0u, std::memory_order_relaxed);
				m_priority = {};
				m_payload = {};
				m_isCompleted = false;
			}

			JobSystem::JobSystem() :
				m_highPriorityPushIndex(), m_highPriorityPopIndex(), m_highPriorityJobQueue(),
				m_lowPriorityPushIndex(), m_lowPriorityPopIndex(), m_lowPriorityJobQueue(),
				m_controlBlockFreeListMutex(),
				m_controlBlockFreeList(),
				m_controlBlocks(),
				m_waitMutex(),
				m_conditionVariable(),
				m_isRunning(true)
			{
				for (u32 index = 0u; index < k_maxJobControlBlocks; ++index)
				{
					m_controlBlockFreeList.push(index);
				}
			}

			JobSystem::~JobSystem() noexcept
			{
				
			}

			JobHandle JobSystem::Submit(
				const Job& payload,
				const std::vector<JobHandle>& dependecies,
				JobPriority priority
			)
			{
				JobHandle handle = UINT32_MAX;
				{
					std::scoped_lock lock(m_controlBlockFreeListMutex);

					handle = m_controlBlockFreeList.top();
					m_controlBlockFreeList.pop();
				}
				m_controlBlocks[handle].Reset();

				Job jobPayload = {};
				jobPayload = payload;
				jobPayload.m_handle = handle;

				m_controlBlocks[handle].m_priority = priority;

				if (dependecies.empty())
				{
					PushJob(jobPayload, priority);
				}
				else
				{
					auto unresolvedDependecies = static_cast<u32>(dependecies.size());
					for (u32 i = 0u; i < static_cast<u32>(dependecies.size()); ++i)
					{
						const JobHandle parentHandle = dependecies[i];

						{
							std::lock_guard lock(m_controlBlocks[parentHandle].m_mutex);

							if (m_controlBlocks[parentHandle].m_isCompleted)
							{
								--unresolvedDependecies;
							}
							else
							{
								m_controlBlocks[parentHandle].m_dependents.emplace_back(
									handle
								);
							}
						}
					}

					if (unresolvedDependecies == 0)
					{
						PushJob(jobPayload, priority);
					}
					else
					{
						m_controlBlocks[handle].m_unresolvedDependecies.store(
							unresolvedDependecies,
							std::memory_order_relaxed
						);

						m_controlBlocks[handle].m_payload = jobPayload;
					}
				}

				return handle;
			}

			void JobSystem::OnEngineTerminate(const event::EventEngineTerminate* data)
			{
				m_isRunning = false;

				m_conditionVariable.notify_all();
			}

			void JobSystem::Work()
			{
				while (m_isRunning)
				{
					Job job{};

					if (TryPopHigh(job))
					{
						if (job.m_execute)
						{
							job.m_execute(job.m_jobData);
						}

						if (job.m_latch)
						{
							job.m_latch->count_down();
						}

						FinishJob(job);
					}
					else if (TryPopLow(job))
					{
						if (job.m_execute)
						{
							job.m_execute(job.m_jobData);
						}

						if (job.m_latch)
						{
							job.m_latch->count_down();
						}

						FinishJob(job);
					}
					else
					{
						std::unique_lock lock(m_waitMutex);
						m_conditionVariable.wait_for(lock, std::chrono::milliseconds(1));
					}
				}
			}

			void JobSystem::PushJob(const Job& payload, JobPriority priority)
			{
				switch (priority)
				{
				case JobPriority::High:
					PushJobHigh(payload);
					break;
				case JobPriority::Low:
					PushJobLow(payload);
					break;
				}

				m_conditionVariable.notify_one();
			}

			void JobSystem::PushJobHigh(const Job& payload)
			{
				const u32 highPriorityJobIndex = m_highPriorityPushIndex.fetch_add(
					1, std::memory_order_relaxed
				);
				const u32 maskIndex = highPriorityJobIndex & k_queueMask;

				m_highPriorityJobQueue[maskIndex].m_payload = payload;
				m_highPriorityJobQueue[maskIndex].m_isWritten.store(true, std::memory_order_release);
			}

			void JobSystem::PushJobLow(const Job& payload)
			{
				const u32 lowPriorityJobIndex = m_lowPriorityPushIndex.fetch_add(
					1, std::memory_order_relaxed
				);
				const u32 maskIndex = lowPriorityJobIndex & k_queueMask;

				m_lowPriorityJobQueue[maskIndex].m_payload = payload;
				m_lowPriorityJobQueue[maskIndex].m_isWritten.store(true, std::memory_order_release);
			}

			bool JobSystem::TryPopHigh(Job& payload)
			{
				const u32 pushIndex = m_highPriorityPushIndex.load(std::memory_order_relaxed);

				while (true)
				{
					u32 popIndex = m_highPriorityPopIndex.load(std::memory_order_acquire);
					const u32 maskIndex = popIndex & k_queueMask;

					// 仕事なし
					if (popIndex >= pushIndex)
					{
						payload = {};

						return false;
					}

					if (m_highPriorityPopIndex.compare_exchange_weak(popIndex, popIndex + 1, std::memory_order_acquire, std::memory_order_relaxed))
					{
						while (!m_highPriorityJobQueue[maskIndex].m_isWritten.load(std::memory_order_acquire))
						{
							std::this_thread::yield();
						}
						payload = m_highPriorityJobQueue[maskIndex].m_payload;
						
						m_highPriorityJobQueue[maskIndex].m_isWritten.store(
							false, std::memory_order_release
						);

						break;
					}
				}

				return true;
			}

			bool JobSystem::TryPopLow(Job& payload)
			{
				const u32 pushIndex = m_lowPriorityPushIndex.load(std::memory_order_relaxed);

				while (true)
				{
					u32 popIndex = m_lowPriorityPopIndex.load(std::memory_order_acquire);
					const u32 maskIndex = popIndex & k_queueMask;

					// 仕事なし
					if (popIndex >= pushIndex)
					{
						payload = {};

						return false;
					}

					if (m_lowPriorityPopIndex.compare_exchange_weak(popIndex, popIndex + 1, std::memory_order_acquire, std::memory_order_relaxed))
					{
						while (!m_lowPriorityJobQueue[maskIndex].m_isWritten.load(std::memory_order_acquire))
						{
							std::this_thread::yield();
						}

						payload = m_lowPriorityJobQueue[maskIndex].m_payload;

						m_lowPriorityJobQueue[maskIndex].m_isWritten.store(
							false, std::memory_order_release
						);

						break;
					}
				}

				return true;
			}

			void JobSystem::FinishJob(const Job& payload)
			{
				JobControlBlock& block = m_controlBlocks[payload.m_handle];

				std::vector<JobHandle> dependependentsCopy{};
				{
					std::lock_guard lock(block.m_mutex);

					block.m_isCompleted = true;
					dependependentsCopy = block.m_dependents;
				}

				for (u32 i = 0u; i < static_cast<u32>(dependependentsCopy.size()); ++i)
				{
					const JobHandle child = dependependentsCopy[i];

					const u32 dependeciesCount = m_controlBlocks[child].m_unresolvedDependecies.fetch_sub(
						1, std::memory_order_relaxed
					);

					// 前回の値が1なら未解決の依存先はない
					if (dependeciesCount == 1u)
					{
						PushJob(m_controlBlocks[child].m_payload, m_controlBlocks[child].m_priority);
					}
				}

				{
					std::scoped_lock lock(m_controlBlockFreeListMutex);

					m_controlBlockFreeList.push(payload.m_handle);
				}
			}
		} // namespace thread
	} // namespace kit
} // namespace ts