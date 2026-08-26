// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RenderCommand/RenderScratchBuffer.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			RenderScratchBuffer::RenderScratchBuffer(const memory::MemoryBlock& memoryBlock) :
				m_memoryBlock(memoryBlock),
				m_currentOffset()
			{
			}

			void* RenderScratchBuffer::AllocateRaw(u64 size, u64 alignmentInBytes)
			{
				const u64 alignedSizeInBytes = memory::AlignUp(
					size, alignmentInBytes
				);

				const u64 totalSize = alignedSizeInBytes + m_memoryBlock.m_currentOffset;
				if (totalSize > m_memoryBlock.m_capacityInBytes)
				{
					TS_FATAL_LOG(
						"メモリサイズを超過しています。\ntotalSize: {}\nCapacity: {}", totalSize, m_memoryBlock.m_capacityInBytes
					);

					return nullptr;
				}

				void* buffer = m_memoryBlock.m_startAddress + m_memoryBlock.m_currentOffset;
				m_memoryBlock.m_currentOffset += alignedSizeInBytes;

				return buffer;
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts