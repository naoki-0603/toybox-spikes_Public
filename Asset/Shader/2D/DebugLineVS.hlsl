// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "DebugLine.hlsli"

#include "../Helper/GraphicsHelper.hlsli"

#include "../SceneConstants.hlsli"

DebugLineVSOutput main(DebugLineVertex input)
{
    DebugLineVSOutput output = (DebugLineVSOutput)0;
    output.m_position = mul(float4(input.m_position, 1.0f), g_viewProjection);
    output.m_color = UnpackColor(input.m_packedColor);

    return output;
}