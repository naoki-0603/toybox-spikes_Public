// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_HELPER_GRAPHICS_HELPER_HLSLI_
#define TS_ASSET_SHADER_HELPER_GRAPHICS_HELPER_HLSLI_

float4 UnpackColor(uint color)
{
	return float4(
		(float)((color >> 0) & 0xFF) / 255.0f,
		(float)((color >> 8) & 0xFF) / 255.0f,
		(float)((color >> 16) & 0xFF) / 255.0f,
		(float)((color >> 24) & 0xFF) / 255.0f
	); 
}

#endif //! TS_ASSET_SHADER_HELPER_GRAPHICS_HELPER_HLSLI_