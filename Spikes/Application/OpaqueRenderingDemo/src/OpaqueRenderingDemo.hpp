// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details.

#ifndef TS_APPLICATION_OPAQUE_RENDERING_DEMO_HPP_
#define TS_APPLICATION_OPAQUE_RENDERING_DEMO_HPP_

#include <Core/Application.hpp>
#include <Core/Thread/JobSystem.hpp>

#include <Graphics/Renderer.hpp>

#include <Asset/AssetManager.hpp>

#include <Asset/Builder/DefaultAssetsBuilder.hpp>
#include <Graphics/Builder/DefaultRenderResourcesBuilder.hpp>

namespace ts
{
	class OpaqueRenderingDemo final : public kit::Application
	{
	public:
		OpaqueRenderingDemo();
		~OpaqueRenderingDemo() noexcept override = default;

	public:
		bool OnInitialize() override;
		bool OnFinalize() override;

		void Update(const kit::ApplicationContext& context);
		void Render(const kit::ApplicationContext& context);
	private:
		void LoadResources();
		
		void CreatePipeline();
		void CreateRenderPasses();
		void CreateRenderPackets();
	private:
		kit::graphics::Renderer* m_renderer;
		kit::asset::AssetManager* m_assetManager;
		kit::asset::DefaultAssetsBuilder* m_defaultAssetBuilder;
		kit::graphics::DefaultRenderResourcesBuilder* m_defaultRenderResourcesBuilder;

		std::vector<kit::StaticMeshAssetHandle> m_sponzaMeshHandles;

		kit::thread::JobSystem m_jobSystem;

		std::vector<kit::TextureAssetHandle> m_sponzaTextureHandles;

		std::vector<kit::graphics::RenderPacket> m_opaqueRenderPackets;
		std::vector<kit::graphics::RenderPacket> m_maskRenderPackets;
		std::vector<kit::graphics::RenderPacket> m_transparentRenderPackets;

		kit::graphics::SceneConstants m_sceneConstants;
	};
} // namespace ts

#endif //! TS_APPLICATION_OPAQUE_RENDERING_DEMO_HPP_