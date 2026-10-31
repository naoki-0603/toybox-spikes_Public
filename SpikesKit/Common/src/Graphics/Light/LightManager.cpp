// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/Light/LightManager.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			LightManager::LightManager() :
				m_directionalLight()
			{
				// ディレクショナルライト
				{
					m_directionalLight.m_color = Vector4(
						1.0f, 1.0f, 1.0f, 1.0f
					);

					m_directionalLight.m_direction.x = 1.0f;
					m_directionalLight.m_direction.y = -1.0f;
					m_directionalLight.m_direction.Normalize();
				}
			}
		} // namespace graphics
	} // namespace kit
} // namespace 