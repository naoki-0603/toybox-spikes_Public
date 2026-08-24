// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef TS_ASSET_SHADER_SCENE_CONSTANTS_HLSLI_
#define TS_ASSET_SHADER_SCENE_CONSTANTS_HLSLI_

cbuffer SceneConstantsBuffer : register(b0)
{
    row_major float4x4 g_viewProjection;
    float4 g_cameraPosition;

    float g_deltaTime;
    float g_totalTime;
    float2 g_scenePadding;
};

#endif //! TS_ASSET_SHADER_SCENE_CONSTANTS_HLSLI_