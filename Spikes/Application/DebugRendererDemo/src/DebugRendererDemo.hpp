// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details.

#ifndef TS_DEBUG_RENDERER_DEMO_HPP_
#define TS_DEBUG_RENDERER_DEMO_HPP_

#include <Core/Application.hpp>

#include <Graphics/RenderPacket.hpp>
#include <Graphics/SceneConstants.hpp>

namespace ts
{
	namespace kit
	{
		namespace graphics
		{
			class Renderer;
			class DebugRenderer;

			class DefaultRenderResourcesBuilder;
		} // namespace graphics

		namespace asset
		{
			class AssetManager;

			class DefaultAssetsBuilder;
		} // namespace asset

		namespace thread
		{
			class JobSystem;
		} // namespace thread
	} // namespace kit
	
	class DebugRendererDemo final : public kit::Application
	{
	public:
		DebugRendererDemo();
		~DebugRendererDemo() noexcept override = default;

	public:
		bool OnInitialize() override;
		bool OnFinalize() override;

		void Update(const kit::ApplicationContext& context);
		void Render(const kit::ApplicationContext& context);

	private:
		[[nodiscard]]
		bool CreateRenderPasses();
	private:
		kit::graphics::Renderer* m_renderer;
		kit::graphics::DebugRenderer* m_debugRenderer;
		kit::graphics::DefaultRenderResourcesBuilder* m_defaultRenderResourceBuilder;

		kit::asset::AssetManager* m_assetManager;
		kit::asset::DefaultAssetsBuilder* m_defaultAssetsBuilder;
		
		kit::thread::JobSystem* m_jobSystem;

		std::vector<kit::graphics::RenderPacket> m_opaqueRenderPackets;
		std::vector<kit::graphics::RenderPacket> m_maskRenderPackets;
		std::vector<kit::graphics::RenderPacket> m_transparentRenderPackets;

		kit::graphics::SceneConstants m_sceneConstants;
	};
} // namespace ts

#endif //! TS_DEBUG_RENDERER_DEMO_HPP_