// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/RenderCommand/RenderResourceRecorder.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			RenderResourceRecorder::RenderResourceRecorder(RenderScratchBuffer* scratchBuffer) :
				m_scratchBuffer(scratchBuffer)
			{

			}
		} // namespace graphics
	} // namespace kit
} // namespace ts