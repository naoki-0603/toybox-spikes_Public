// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_ASSET_MANAGER_HPP_
#define SPIKES_KIT_COMMON_ASSET_ASSET_MANAGER_HPP_

#include "AssetDatabase.hpp"

#include "Graphics/RHI/RHI_Types.hpp"

#include <shared_mutex>

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class RHI_ShaderCompiler;
		} // namepsace graphics

		namespace resource
		{
			class RenderResourceManager;
		} // namespace resource

		namespace asset
		{
			class ShaderImporter;
			class TextureImporter;
			class MeshImporter;

			struct AssetManagerInitDesc final
			{
				resource::RenderResourceManager* m_renderResourceManager{};
				
				graphics::RHI_ShaderCompiler* m_shaderCompiler{};
			};

			struct LoadShaderConfig final
			{
				fs::path m_path{};
				std::string m_entryPoint{};
				std::string m_target{};

				graphics::RHI_ShaderType m_shaderType{};
				graphics::RHI_ShaderCompilerOptions m_options{};

				std::vector<graphics::RHI_ShaderMacro> m_macros{};
			};

			struct LoadTextureConfig final
			{
				fs::path m_path{};
			};

			struct LoadMeshConfig final
			{
				fs::path m_path{};

				ShaderAssetHandle m_staticMeshVertexShaderHandle{};
				ShaderAssetHandle m_meshPBRPixelShaderHandle{};

				// 左手系に変換するか
				bool m_convertToLeftHanded{};
			};

			class AssetManager final
			{
			public:
				AssetManager();
				AssetManager(const AssetManager&) = delete;
				AssetManager(AssetManager&&) noexcept = delete;
				~AssetManager() noexcept = default;

				AssetManager& operator=(const AssetManager&) = delete;
				AssetManager& operator=(AssetManager&&) noexcept = delete;
			public:
				[[nodiscard]]
				bool Create(const AssetManagerInitDesc& desc);

				[[nodiscard]]
				bool Destroy();

			public:
				[[nodiscard]]
				const ShaderAsset* FindShader(ShaderAssetHandle handle) const;

				[[nodiscard]]
				const TextureAsset* FindTexture(TextureAssetHandle handle) const;

				[[nodiscard]]
				const MaterialAsset* FindMaterial(MaterialAssetHandle handle) const;

				[[nodiscard]]
				const StaticMeshAsset* FindStaticMesh(StaticMeshAssetHandle handle) const;

			public:
				[[nodiscard]]
				ShaderAssetHandle LoadShader(const LoadShaderConfig& config);

				[[nodiscard]]
				TextureAssetHandle LoadTexture(const LoadTextureConfig& config);

				[[nodiscard]]
				std::vector<StaticMeshAssetHandle> LoadStaticMesh(const LoadMeshConfig& config);
			private:
				AssetDatabase m_database;

				resource::RenderResourceManager* m_renderResourceManager;
				ShaderImporter* m_shaderImporter;
				TextureImporter* m_textureImporter;
				MeshImporter* m_meshImporter;

				mutable std::shared_mutex m_loadShaderMutex;
				mutable std::shared_mutex m_loadTextureMutex;
				mutable std::shared_mutex m_loadMaterialMutex;
				mutable std::shared_mutex m_loadStaticMeshMutex;
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_ASSET_MANAGER_HPP_