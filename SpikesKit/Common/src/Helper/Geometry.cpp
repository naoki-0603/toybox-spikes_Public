// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Helper/Geometry.hpp"

namespace ts
{
	namespace kit
	{
		Geometry::Geometry() noexcept :
			m_vertices(), m_indices()
		{
		}

		Geometry CreateTriangle() noexcept
		{
			Geometry triangle = {};
			triangle.m_vertices.resize(4u);
			{
				triangle.m_vertices[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
				triangle.m_vertices[1] = { +0.0f, +0.5f, 0.0f, 1.0f };
				triangle.m_vertices[2] = { +0.5f, -0.5f, 0.0f, 1.0f };
				triangle.m_vertices[3] = { +0.0f, -0.5f, 0.0f, 1.0f };
			}

			triangle.m_indices.resize(6u);
			{
				triangle.m_indices[0] = 0;
				triangle.m_indices[1] = 1;
				triangle.m_indices[2] = 3;

				triangle.m_indices[3] = 3;
				triangle.m_indices[4] = 1;
				triangle.m_indices[5] = 2;
			}

			return triangle;
		}
	} // namespace kit
} // namespace ts