// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_OBJECT_CONSTANTS_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_OBJECT_CONSTANTS_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			struct alignas(256) ObjectConstants final
			{
				Matrix m_worldTransform{};
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_OBJECT_CONSTANTS_HPP_