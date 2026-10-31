// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_HELPER_GRAPHICS_HELPER_HPP_
#define SPIKES_KIT_COMMON_HELPER_GRAPHICS_HELPER_HPP_

namespace ts
{
	namespace kit
	{
		namespace graphicshelper
		{
			[[nodiscard]]
			constexpr u8 F32ToU8SDR(f32 value) noexcept
			{
				return static_cast<u8>(std::clamp(value * 255.0f, 0.0f, 255.0f));
			}

			// RGBA(Vector4(f32)) -> u32
			[[nodiscard]]
			constexpr u32 PackColor(const Vector4& color) noexcept
			{
				return (static_cast<u32>(F32ToU8SDR(color.x)) << 0)  |
					   (static_cast<u32>(F32ToU8SDR(color.y)) << 8)  |
					   (static_cast<u32>(F32ToU8SDR(color.z)) << 16) |
					   (static_cast<u32>(F32ToU8SDR(color.w)) << 24);
			}

			// TODO(naoki): constexpr化を行う（Vector4のコンストラクタがconstexpr対応していないため今はできない）
			[[nodiscard]]
			inline Vector4 UnpackColor(u32 color) noexcept
			{
				return Vector4(
					static_cast<f32>((color >> 0) & 0xFF) / 255.0f,
					static_cast<f32>((color >> 8) & 0xFF) / 255.0f,
					static_cast<f32>((color >> 16) & 0xFF) / 255.0f,
					static_cast<f32>((color >> 24) & 0xFF) / 255.0f
				); 
			}
		} // namespace graphicshelper
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_HELPER_GRAPHICS_HELPER_HPP_