// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

struct VSInput
{
    float4 m_position : POSITION;
};

struct VSOutput
{
    float4 m_position : SV_Position;
};

cbuffer Constants : register(b0)
{
    row_major float4x4 k_world;
    row_major float4x4 k_viewProjection;
};