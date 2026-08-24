// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_MESH_MESH_HLSLI_
#define TS_ASSET_SHADER_MESH_MESH_HLSLI_

// ピクセル入力
struct MeshVertexOut
{
    float4 m_position : SV_POSITION;
    float4 m_worldPosition : POSITION;
    float4 m_normal : NORMAL;
    float2 m_texcoord0 : TEXCOORD0;
    float2 m_texcoord1 : TEXCOORD1;
};

// オブジェクト定数バッファ
cbuffer ObjectConstantsBuffer : register(b1)
{
    row_major float4x4 g_worldTransform;
};

#endif //! TS_ASSET_SHADER_MESH_MESH_HLSLI_