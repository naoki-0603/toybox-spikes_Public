// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Mesh.hlsli"
#include "PBRMaterial.hlsli"

Texture2D g_baseColorTexture : register(t0);
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

    const float2 texcoord = g_baseColorTextureTexcoord == 0 ?
        output.m_texcoord0 :
        output.m_texcoord1;

    finalColor = g_baseColorTexture.Sample(g_pointSampler, texcoord) * g_baseColorFactor;

#ifdef USE_MASK
    clip(finalColor.a - g_alphaCutoff);
#endif // USE_MASK

    return finalColor;
}