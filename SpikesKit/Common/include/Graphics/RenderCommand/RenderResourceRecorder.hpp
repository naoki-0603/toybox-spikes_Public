// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_RESOURCE_RECORDER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_RESOURCE_RECORDER_HPP_

#include "RenderScratchBuffer.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class RenderResourceRecorder final
			{
			public:
				RenderResourceRecorder() = default;
				RenderResourceRecorder(RenderScratchBuffer* scratchBuffer);
				RenderResourceRecorder(const RenderResourceRecorder&) = default;
				RenderResourceRecorder(RenderResourceRecorder&&) noexcept = default;
				~RenderResourceRecorder() noexcept = default;

				RenderResourceRecorder& operator=(const RenderResourceRecorder&) = default;
				RenderResourceRecorder& operator=(RenderResourceRecorder&&) noexcept = default;
			public:
				template <class T>
				[[nodiscard]]
				T* AllocateObject(u64 count = 1u, u64 alignmentInBytes = 16u)
				{
					void* ptr = m_scratchBuffer->AllocateRaw(sizeof(T) * count, alignmentInBytes);

					return new(ptr) T();
				}

				template <class T>
				[[nodiscard]]
				T* AllocateArray(u64 count = 1u, u64 alignmentInBytes = 16u)
				{
					void* ptr = m_scratchBuffer->AllocateRaw(sizeof(T) * count, alignmentInBytes);

					return reinterpret_cast<T*>(ptr);
				}
			private:
				RenderScratchBuffer* m_scratchBuffer;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_RESOURCE_RECORDER_HPP_