// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_MESH_STATIC_MESH_HLSLI_
#define TS_ASSET_SHADER_MESH_STATIC_MESH_HLSLI_

// 頂点フォーマット
struct StaticMeshVertex
{
    float4 m_position : POSITION;
    float4 m_normal : NORMAL;
    float4 m_tangent : TANGENT;

    float2 m_texcoord0 : TEXCOORD0;
    float2 m_texcoord1 : TEXCOORD1;
};

#endif //! TS_ASSET_SHADER_MESH_STATIC_MESH_HLSLI_