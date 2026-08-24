// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "StaticMesh.hlsli"
#include "Mesh.hlsli"

#include "../SceneConstants.hlsli"

MeshVertexOut main(StaticMeshVertex input)
{
    MeshVertexOut output = (MeshVertexOut)0;
    output.m_worldPosition = mul(input.m_position, g_worldTransform);
    output.m_position = mul(output.m_worldPosition, g_viewProjection);

    output.m_normal = input.m_normal;

    output.m_texcoord0 = input.m_texcoord0;
    output.m_texcoord1 = input.m_texcoord1;

    return output;
}