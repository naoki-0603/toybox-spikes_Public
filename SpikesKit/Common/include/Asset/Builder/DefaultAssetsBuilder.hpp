// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_ASSET_BUILDER_DEFAULT_ASSETS_BUILDER_HPP_
#define SPIKES_KIT_COMMON_ASSET_BUILDER_DEFAULT_ASSETS_BUILDER_HPP_

namespace ts
{
	namespace kit
	{
		namespace asset
		{
			class AssetManager;

			class DefaultAssetsBuilder final
			{
			public:
				DefaultAssetsBuilder();
				DefaultAssetsBuilder(const DefaultAssetsBuilder&) = delete;
				DefaultAssetsBuilder(DefaultAssetsBuilder&&) noexcept = default;
				~DefaultAssetsBuilder() noexcept = default;

			public:
				[[nodiscard]]
				bool Build(AssetManager* assetManager);

			public:
				[[nodiscard]]
				ShaderAssetHandle GetStaticMeshVertexShaderHandle() const noexcept { return m_staticMeshVertexShaderHandle; }

				[[nodiscard]]
				ShaderAssetHandle GetMeshPBRPixelShaderHandle() const noexcept { return m_meshPBRPixelShaderHandle; }

				[[nodiscard]]
				ShaderAssetHandle GetMeshPBRWithMaskPixelShaderHandle() const noexcept { return m_meshPBRWithMaskPixelShaderHandle; }

				[[nodiscard]]
				ShaderAssetHandle GetDebugLineVertexShaderHandle() const noexcept { return m_debugLineVertexShaderHandle; }

				[[nodiscard]]
				ShaderAssetHandle GetDebugLinePixelShaderHandle() const noexcept { return m_debugLinePixelShaderHandle; }
			private:
				ShaderAssetHandle m_staticMeshVertexShaderHandle;
				ShaderAssetHandle m_meshPBRPixelShaderHandle;
				ShaderAssetHandle m_meshPBRWithMaskPixelShaderHandle;
				ShaderAssetHandle m_debugLineVertexShaderHandle;
				ShaderAssetHandle m_debugLinePixelShaderHandle;
			};
		} // namespace asset
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_ASSET_BUILDER_DEFAULT_ASSETS_BUILDER_HPP_