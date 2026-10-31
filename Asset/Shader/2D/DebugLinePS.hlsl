// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "DebugLine.hlsli"

float4 main(DebugLineVSOutput output) : SV_TARGET
{
    return output.m_color;
}