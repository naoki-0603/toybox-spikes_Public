// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_SCRATCH_BUFFER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_SCRATCH_BUFFER_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class RenderScratchBuffer final
			{
			public:
				RenderScratchBuffer() = default;
				RenderScratchBuffer(const memory::MemoryBlock& memoryBlock);
				RenderScratchBuffer(const RenderScratchBuffer&) = default;
				RenderScratchBuffer(RenderScratchBuffer&&) noexcept = default;
				~RenderScratchBuffer() noexcept = default;

				RenderScratchBuffer& operator=(const RenderScratchBuffer&) = default;
				RenderScratchBuffer& operator=(RenderScratchBuffer&&) noexcept = default;
			public:
				[[nodiscard]]
				void* AllocateRaw(u64 size, u64 alignmentInBytes);

			private:
				memory::MemoryBlock m_memoryBlock;

				u64 m_currentOffset;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_RENDER_COMMAND_RENDER_SCRATCH_BUFFER_HPP_