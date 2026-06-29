// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_HELPER_GEOMETRY_HPP_
#define SPIKES_KIT_COMMON_HELPER_GEOMETRY_HPP_

namespace ts
{
	namespace kit
	{
		struct Geometry final
		{
		public:
			Geometry() noexcept;
			~Geometry() noexcept = default;

		public:
			std::vector<Vector4> m_vertices;
			std::vector<u32> m_indices;
		};

		[[nodiscard]]
		Geometry CreateTriangle() noexcept;
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_HELPER_GEOMETRY_HPP_