// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_2D_DEBUG_LINE_HLSLI_
#define TS_ASSET_SHADER_2D_DEBUG_LINE_HLSLI_

struct DebugLineVertex
{
    float3 m_position : POSITION;
    uint m_packedColor : COLOR;
};

struct DebugLineVSOutput
{
    float4 m_position : SV_POSITION;
    float4 m_color : COLOR;
};

#endif //! TS_ASSET_SHADER_2D_DEBUG_LINE_HLSLI_