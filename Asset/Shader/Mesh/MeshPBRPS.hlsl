// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Mesh.hlsli"
#include "PBRMaterial.hlsli"

#include "../SceneConstants.hlsli"

Texture2D g_baseColorTexture : register(t0);
Texture2D g_normalTexture : register(t2);
SamplerState g_pointSampler : register(s0);

float4 main(MeshVertexOut output) : SV_TARGET
{
    float4 finalColor = g_baseColorFactor;

    if (g_baseColorTextureTexcoord == -1)
    {
#ifdef USE_MASK
    clip(g_baseColorFactor.a - g_alphaCutoff);
#endif // USE_MASK
        return g_baseColorFactor;
    }

    // BaseColor
    const float2 texcoord = g_baseColorTextureTexcoord == 0 ?
        output.m_texcoord0 :
        output.m_texcoord1;

    finalColor = g_baseColorTexture.Sample(g_pointSampler, texcoord) * g_baseColorFactor;

#ifdef USE_MASK
    clip(finalColor.a - g_alphaCutoff);
#endif // USE_MASK

    // Normal
    // if (g_normalTextureTexcoord != -1)
    // {
    //     const float2 texcoord = g_normalTextureTexcoord == 0 ?
    //         output.m_texcoord0 :
    //         output.m_texcoord1;
// 
    //     return g_normalTexture.Sample(g_pointSampler, texcoord);
    // }

    // Lighting
    // float3 normal = normalize(output.m_normal.xyz);
    // float3 directionalLightDirection = normalize(g_directionalLightDirection);
    // float NDotL = dot(normal, directionalLightDirection);
    // float halfLambert = NDotL * 0.5f + 0.5f;
    // halfLambert = halfLambert * halfLambert;
// 
    // float3 lighting = finalColor.rgb * g_directionalLightColor.rgb * halfLambert;
    // finalColor = float4(lighting.rgb, finalColor.w);
    
    return finalColor;
}