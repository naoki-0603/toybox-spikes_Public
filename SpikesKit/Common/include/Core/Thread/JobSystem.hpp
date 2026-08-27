// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_THREAD_JOB_SYSTEM_HPP_
#define SPIKES_KIT_COMMON_CORE_THREAD_JOB_SYSTEM_HPP_

#include <stack>

namespace ts
{
	namespace kit
	{
		namespace event
		{
			struct EventEngineTerminate;
		} // namespace event

		namespace thread
		{
			using JobHandle = u32;

			enum class JobPriority : i32
			{
				High,
				Low,

				Max
			};
			TS_DEFINE_ENUM_BIT_OPERATORS(JobPriority)

			struct Job final
			{
				using Task = void (*)(void*); /// void Job(void*) { }
				Task m_execute{};
				void* m_jobData{};
				std::latch* m_latch{}; /// メインスレッドの待機に使用
				JobHandle m_handle{};
			};

			struct JobQueueEntry final
			{
				Job m_payload{};
				std::atomic<bool> m_isWritten{ false }; // 書きこまれてたか
			};

			struct JobControlBlock final
			{
				void Reset();
				
				std::vector<JobHandle> m_dependents{};
				std::atomic<u32> m_unresolvedDependecies{};
				JobPriority m_priority{};
				Job m_payload{};

				std::mutex m_mutex{};
				bool m_isCompleted{};
			};

			class JobSystem final
			{
				static constexpr u32 k_maxJobs = 512u;
				static constexpr u32 k_maxJobControlBlocks = 512u;
				static constexpr u32 k_queueMask = 511u;
			public:
				JobSystem();
				JobSystem(const JobSystem&) = delete;
				JobSystem(JobSystem&&) noexcept = default;
				~JobSystem() noexcept;

				JobSystem& operator=(const JobSystem&) = delete;
				JobSystem& operator=(JobSystem&&) noexcept = default;
			public:
				JobHandle Submit(
					const Job& payload,
					const std::vector<JobHandle>& dependents,
					JobPriority priority
				);

			public:
				void OnEngineTerminate(const event::EventEngineTerminate* data);
			
				/// ワーカー用
				void Work();
			private:
				void PushJob(const Job& payload, JobPriority priority);

				void PushJobHigh(const Job& payload);
				void PushJobLow(const Job& payload);

				[[nodiscard]]
				bool TryPopHigh(Job& payload);

				[[nodiscard]]
				bool TryPopLow(Job& payload);

				void FinishJob(const Job& payload);
			private:
				std::atomic<u32> m_highPriorityPushIndex;
				std::atomic<u32> m_highPriorityPopIndex;
				JobQueueEntry m_highPriorityJobQueue[k_maxJobs];

				std::atomic<u32> m_lowPriorityPushIndex;
				std::atomic<u32> m_lowPriorityPopIndex;
				JobQueueEntry m_lowPriorityJobQueue[k_maxJobs];

				std::mutex m_controlBlockFreeListMutex;
				std::stack<u32> m_controlBlockFreeList;
				JobControlBlock m_controlBlocks[k_maxJobControlBlocks];

				std::mutex m_waitMutex;
				std::condition_variable m_conditionVariable;
				std::atomic<bool> m_isRunning;
			};
		} // namespace thread
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_THREAD_JOB_SYSTEM_HPP_