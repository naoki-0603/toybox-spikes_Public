// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_GRAPHICS_LIGHT_LIGHT_MANAGER_HPP_
#define SPIKES_KIT_COMMON_GRAPHICS_LIGHT_LIGHT_MANAGER_HPP_

#include "Light.hpp"

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class LightManager final
			{
			public:
				LightManager();
				LightManager(const LightManager&) = delete;
				LightManager(LightManager&&) noexcept = default;
				~LightManager() noexcept = default;

			public:
				[[nodiscard]]
				const DirectionalLight& GetDirectionalLight() const noexcept { return m_directionalLight; }

			private:
				DirectionalLight m_directionalLight;
			};
		} // namespace graphics
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_GRAPHICS_LIGHT_LIGHT_MANAGER_HPP_