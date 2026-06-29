// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Triangle.hlsli"

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;
    output.m_position = mul(input.m_position, k_world);
    output.m_position = mul(output.m_position, k_viewProjection);
    
    return output;
}