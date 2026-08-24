// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Asset/Builder/DefaultAssetsBuilder.hpp"

#include "Asset/AssetManager.hpp"

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			DefaultAssetsBuilder::DefaultAssetsBuilder() :
				m_staticMeshVertexShaderHandle(),
				m_meshPBRPixelShaderHandle(),
				m_meshPBRWithMaskPixelShaderHandle()
			{
			}

			bool DefaultAssetsBuilder::Build(AssetManager* assetManager)
			{
				using namespace graphics;

				const fs::path k_rootDirectory = fs::GetRootDirectory();

				// Shaders
				{
					RHI_ShaderCompilerOptions options = {};
#if TS_DEBUG
					options = RHI_ShaderCompilerOptions::EnableDebug;
#else // TS_DEBUG
					options = RHI_ShaderCompilerOptions::Default;
#endif //! TS_DEBUG
					// StaticMeshVS
					{
						LoadShaderConfig staticMeshVertexShaderConfig = {};
						staticMeshVertexShaderConfig.m_entryPoint = "main";
						staticMeshVertexShaderConfig.m_options = options;
						staticMeshVertexShaderConfig.m_shaderType = RHI_ShaderType::VertexShader;
						staticMeshVertexShaderConfig.m_target = "vs_5_0";
						staticMeshVertexShaderConfig.m_path = fs::path(k_rootDirectory / "Asset/Shader/Mesh/StaticMeshVS.hlsl").lexically_normal();

						m_staticMeshVertexShaderHandle = assetManager->LoadShader(staticMeshVertexShaderConfig);
					}

					// MeshPBR
					{
						LoadShaderConfig meshPBRPixelShaderConfig = {};
						meshPBRPixelShaderConfig.m_entryPoint = "main";
						meshPBRPixelShaderConfig.m_options = options;
						meshPBRPixelShaderConfig.m_shaderType = RHI_ShaderType::PixelShader;
						meshPBRPixelShaderConfig.m_target = "ps_5_0";
						meshPBRPixelShaderConfig.m_path = fs::path(k_rootDirectory / "Asset/Shader/Mesh/MeshPBRPS.hlsl").lexically_normal();

						m_meshPBRPixelShaderHandle = assetManager->LoadShader(meshPBRPixelShaderConfig);
					}

					// MeshPBR(WithMask)
					{
						LoadShaderConfig meshPBRWithMaskPixelShaderConfig = {};
						meshPBRWithMaskPixelShaderConfig.m_entryPoint = "main";
						meshPBRWithMaskPixelShaderConfig.m_options = options;
						meshPBRWithMaskPixelShaderConfig.m_shaderType = RHI_ShaderType::PixelShader;
						meshPBRWithMaskPixelShaderConfig.m_target = "ps_5_0";
						meshPBRWithMaskPixelShaderConfig.m_path = fs::path(k_rootDirectory / "Asset/Shader/Mesh/MeshPBRPS.hlsl").lexically_normal();
						
						RHI_ShaderMacro useMaskMacro = {};
						useMaskMacro.m_name = "USE_MASK";
						useMaskMacro.m_definition = "1";
						meshPBRWithMaskPixelShaderConfig.m_macros.push_back(std::move(useMaskMacro));

						m_meshPBRWithMaskPixelShaderHandle = assetManager->LoadShader(meshPBRWithMaskPixelShaderConfig);
					}
				}

				return true;
			}
		} // namespace asset
	} // namespace kit
} // namespace ts
