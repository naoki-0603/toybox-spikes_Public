// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_MESH_PBR_MATERIAL_HLSLI_
#define TS_ASSET_SHADER_MESH_PBR_MATERIAL_HLSLI_

cbuffer PBRMaterialConstantBuffer : register(b2)
{
    float4 g_baseColorFactor;

    float g_alphaCutoff;
    int g_doubleSided;
    int g_alphaMode;
    float g_metallicFactor;
    
    float g_roughnessFactor;
    float g_normalScale;
    float g_occlusionStrength;
    float g_padding;

    float3 g_emissiveFactor;
    float g_padding2;

    int g_baseColorTextureIndex;
    int g_baseColorSamplerIndex;
    int g_baseColorTextureTexcoord;

    int g_metallicRoughnessTextureIndex;
    int g_metallicRoughnessSamplerIndex;
    int g_metallicRoughnessTextureTexcoord;

    int g_normalTextureIndex;
    int g_normalTextureSamplerIndex;
    int g_normalTextureTexcoord;

    int g_occlusionTextureIndex;
    int g_occlusionTextureSamplerIndex;
    int g_occlusionTextureTexcoord;

    int g_emissiveTextureIndex;
    int g_emissiveTextureSamplerIndex;
    int g_emissiveTextureTexcoord;

    float g_padding3;
};

#endif //! TS_ASSET_SHADER_MESH_PBR_MATERIAL_HLSLI_