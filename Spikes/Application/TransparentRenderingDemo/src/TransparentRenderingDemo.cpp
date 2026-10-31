// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "TransparentRenderingDemo.hpp"

#include <Graphics/Renderer.hpp>

#include <Graphics/RenderPass/PresentationSetupRenderPass.hpp>
#include <Graphics/RenderPass/OpaqueRenderPass.hpp>
#include <Graphics/RenderPass/MaskRenderPass.hpp>
#include <Graphics/RenderPass/TransparentRenderPass.hpp>

#include <Graphics/Builder/DefaultRenderResourcesBuilder.hpp>

#include <Asset/AssetManager.hpp>
#include <Asset/Builder/DefaultAssetsBuilder.hpp>

#include <Asset/StaticMeshAsset.hpp>

#include <Core/Thread/JobSystem.hpp>

namespace ts
{
	TransparentRenderingDemo::TransparentRenderingDemo() :
		Application("AlphaModeRendererApplication", IVector2(640, 480)),
		m_renderer(),
		m_assetManager(),
		m_defaultRenderResourceBuilder(),
		m_defaultAssetsBuilder(),
		m_jobSystem(),
		m_alphaModeTestTextureHandles(),
		m_alphaModeTestMeshHandles(),
		m_opaqueRenderPackets(),
		m_maskRenderPackets(),
		m_transparentRenderPackets(),
		m_sceneConstants()
	{
	}

	bool TransparentRenderingDemo::OnInitialize()
	{
		// レンダラーの作成
		{
			m_renderer = new kit::graphics::Renderer();

			kit::graphics::RendererInitDesc desc = {};
			desc.m_windowSize = m_window.GetSize();
			desc.m_windowHandle = m_window.GetHandle();
			desc.m_windowResizeEventDispatcher = m_window.GetResizeEventDispatcher();
			desc.m_engineTerminateEventDispatcher = &m_terminateEventDispatcher;
			desc.m_threadManager = &m_threadManager;

			if (!m_renderer->Create(desc))
			{
				TS_FATAL_LOG("Rendererの作成に失敗しました。");

				return false;
			}
		}

		// アセットマネージャーの作成
		{
			kit::asset::AssetManagerInitDesc desc = {};
			desc.m_renderResourceManager = m_renderer->GetRenderResourceManager();
			desc.m_shaderCompiler = m_renderer->GetShaderCompiler();

			m_assetManager = new kit::asset::AssetManager;
			if (!m_assetManager->Create(desc))
			{
				TS_FATAL_LOG("AssetManagerの作成に失敗しました。");

				return false;
			}
		}

		// ジョブシステムの作成とワーカースレッドの登録
		{
			m_jobSystem = new kit::thread::JobSystem;

			if (!m_threadManager.RegisterWorkersFunc(
				[](void* jobSystem) -> void
				{
					auto system = reinterpret_cast<kit::thread::JobSystem*>(jobSystem);
					system->Work();
				},
				m_jobSystem,
				kit::thread::WorkerType::Common
			))
			{
				TS_FATAL_LOG("ワーカースレッドの登録に失敗しました。");

				return false;
			}
		}

		// デフォルトアセットの作成
		{
			m_defaultAssetsBuilder = new kit::asset::DefaultAssetsBuilder;
			
			if (!m_defaultAssetsBuilder->Build(m_assetManager))
			{
				TS_FATAL_LOG("デフォルトアセットの作成に失敗しました。");

				return false;
			}
		}

		// デフォルトレンダーリソースの作成
		{
			const kit::asset::ShaderAsset* staticMeshVertexShader = m_assetManager->FindShader(
				m_defaultAssetsBuilder->GetStaticMeshVertexShaderHandle()
			);

			const kit::asset::ShaderAsset* meshPBRPixelShader = m_assetManager->FindShader(
				m_defaultAssetsBuilder->GetMeshPBRPixelShaderHandle()
			);

			const kit::asset::ShaderAsset* meshPBRWithMaskPixelShader = m_assetManager->FindShader(
				m_defaultAssetsBuilder->GetMeshPBRWithMaskPixelShaderHandle()
			);

			m_defaultRenderResourceBuilder = new kit::graphics::DefaultRenderResourcesBuilder;

			kit::graphics::DefaultRenderResourcesBuilderDesc desc = {};
			desc.m_renderResourceManager = m_renderer->GetRenderResourceManager();
			desc.m_staticMeshVertexShaderHandle = staticMeshVertexShader->m_resourceHandle;
			desc.m_meshPBRPixelShaderHandle = meshPBRPixelShader->m_resourceHandle;
			desc.m_meshPBRWithMaskPixelShaderHandle = meshPBRWithMaskPixelShader->m_resourceHandle;

			if (!m_defaultRenderResourceBuilder->Build(desc))
			{
				TS_FATAL_LOG("デフォルトレンダーリソースの作成に失敗しました。");

				return false;
			}
		}

		{
			m_terminateEventDispatcher.RegisterListener(
				TS_BIND_EVENT(
					m_jobSystem,
					kit::thread::JobSystem,
					OnEngineTerminate,
					kit::event::EventEngineTerminate,
					0u
				)
			);
		}

		if (!LoadResources())
		{
			TS_FATAL_LOG("リソースの読み込みに失敗しました。");

			return false;
		}

		if (!CreateRenderPasses())
		{
			TS_FATAL_LOG("RenderPassの作成に失敗しました。");

			return false;
		}

		if (!BuildRenderPackets())
		{
			TS_FATAL_LOG("RenderPacketsの作成に失敗しました。");

			return false;
		}

		return true;
	}

	bool TransparentRenderingDemo::OnFinalize()
	{
		TS_SAFE_RELEASE(m_defaultRenderResourceBuilder);
		TS_SAFE_RELEASE(m_defaultAssetsBuilder);
		TS_SAFE_RELEASE(m_jobSystem);

		if (!m_renderer->Destroy())
		{
			TS_FATAL_LOG("Rendererの破棄に失敗しました。");

			return false;
		}

		TS_SAFE_RELEASE(m_renderer);
		
		return true;
	}

	void TransparentRenderingDemo::Update(const kit::ApplicationContext& context)
	{
		Application::Update(context);
	
		// 定数の更新
		{
			m_sceneConstants.m_cameraPosition = Vector4(m_camera->GetEye(), 1.0f);
			m_sceneConstants.m_viewProjection = m_camera->GetViewProjection();
			m_sceneConstants.m_deltaTime = context.m_timerRef.GetDeltaTime();
			m_sceneConstants.m_totalTime = context.m_timerRef.GetTime();
		}

		// ウィンドウタイトルの更新
		{
			const std::string title = "AlphaModeRendererApplication (FPS: " + 
				std::to_string(context.m_timerRef.GetFPS()) + ")";

			m_window.SetWindowTitle(title);
		}
	}

	void TransparentRenderingDemo::Render(const kit::ApplicationContext& context)
	{
		Application::Render(context);

		m_renderer->BeginFrame(m_sceneConstants);
		
		kit::graphics::RenderPacketsBundle bundle = {};
		bundle.m_opaqueRenderPackets = m_opaqueRenderPackets;
		bundle.m_maskRenderPackets = m_maskRenderPackets;
		bundle.m_transparentRenderPackets = m_transparentRenderPackets;

		m_renderer->Render(bundle);
		m_renderer->EndFrame();
		m_renderer->Present();
	}

	bool TransparentRenderingDemo::LoadResources()
	{
		// テクスチャのロード
		{
			std::vector<kit::fs::path> paths = {};

			for (auto&& file : kit::fs::recursive_directory_iterator(kit::fs::path(kit::fs::GetRootDirectory() / "Asset/Mesh/AlphaBlendModeTest/")))
			{
				if (file.path().extension() != ".png" &&
					file.path().extension() != ".jpg")
				{
					continue;
				}

				paths.emplace_back(file.path().lexically_normal());
			}

			m_alphaModeTestTextureHandles.resize(paths.size());
			for (u64 i = 0u; i < m_alphaModeTestTextureHandles.size(); ++i)
			{
				kit::asset::LoadTextureConfig config = {};
				config.m_path = paths[i];

				m_alphaModeTestTextureHandles[i] = m_assetManager->LoadTexture(
					config
				);
			}
		}

		// メッシュのロード
		{
			kit::asset::LoadMeshConfig config = {};
			config.m_path = kit::fs::path(kit::fs::GetRootDirectory() / "Asset/Mesh/AlphaBlendModeTest/AlphaBlendModeTest.gltf").lexically_normal();
			config.m_staticMeshVertexShaderHandle = m_defaultAssetsBuilder->GetStaticMeshVertexShaderHandle();
			config.m_meshPBRPixelShaderHandle = m_defaultAssetsBuilder->GetMeshPBRPixelShaderHandle();
			config.m_convertToLeftHanded = true;

			m_alphaModeTestMeshHandles = m_assetManager->LoadStaticMesh(config);
		}

		return true;
	}

	bool TransparentRenderingDemo::CreateRenderPasses()
	{
		// PresentationSetupRenderPass
		{
			kit::graphics::RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 0u;

			auto presentationSetupRenderPass = new kit::graphics::PresentationSetupRenderPass();
			if (!presentationSetupRenderPass->Create(renderPassDesc))
			{
				return false;
			}

			m_renderer->AddRenderPass(presentationSetupRenderPass);
		}

		// OpaqueRenderPass
		{
			kit::graphics::RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 1u;

			auto opaqueRenderPass = new kit::graphics::OpaqueRenderPass(
				m_defaultRenderResourceBuilder->GetOpaquePipelineStateHandle()
			);
			if (!opaqueRenderPass->Create(renderPassDesc))
			{
				return false;
			}

			m_renderer->AddRenderPass(opaqueRenderPass);
		}

		// MaskRenderPass
		{
			kit::graphics::RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 2u;

			auto maskRenderPass = new kit::graphics::MaskRenderPass(
				m_defaultRenderResourceBuilder->GetMaskPipelineStateHandle()
			);
			if (!maskRenderPass->Create(renderPassDesc))
			{
				return false;
			}

			m_renderer->AddRenderPass(maskRenderPass);
		}

		// TransparentRenderPass
		{
			kit::graphics::RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 3u;

			auto transparentRenderPass = new kit::graphics::TransparentRenderPass(
				m_defaultRenderResourceBuilder->GetTransparentPipelineStateHandle()
			);
			if (!transparentRenderPass->Create(renderPassDesc))
			{
				return false;
			}

			m_renderer->AddRenderPass(transparentRenderPass);
		}
		
		return true;
	}

	bool TransparentRenderingDemo::BuildRenderPackets()
	{
		for (u64 meshIndex = 0u; meshIndex < m_alphaModeTestMeshHandles.size(); ++meshIndex)
		{
			const kit::asset::StaticMeshAsset* mesh = m_assetManager->FindStaticMesh(
				m_alphaModeTestMeshHandles[meshIndex]
			);

			for (u64 subMeshIndex = 0u; subMeshIndex < mesh->m_subMeshes.size(); ++subMeshIndex)
			{
				// アセットの取得
				const kit::asset::StaticMeshAsset::SubMesh& subMesh = mesh->m_subMeshes[subMeshIndex];
				const kit::asset::MaterialAsset* material = m_assetManager->FindMaterial(subMesh.m_materialAssetHandle);
				const kit::asset::ShaderAsset* pixelShader = m_assetManager->FindShader(material->m_shaderAssetHandle);

				// AlphaModeによって変更する
				kit::graphics::RenderPacket* renderPacket = nullptr;
				switch (material->m_alphaMode)
				{
				case kit::asset::MaterialAlphaMode::Opaque:
					{
						renderPacket = &m_opaqueRenderPackets.emplace_back();
					}
					break;
				case kit::asset::MaterialAlphaMode::Mask:
					{
						renderPacket = &m_maskRenderPackets.emplace_back();
					}
					break;
				case kit::asset::MaterialAlphaMode::Blend:
					{
						renderPacket = &m_transparentRenderPackets.emplace_back();
					}
					break;
				}

				// バッファ群とシェーダーの設定
				renderPacket->m_worldTransform = subMesh.m_worldTransform;
				renderPacket->m_vertexBufferHandle = subMesh.m_vertexBufferHandle;
				renderPacket->m_indexBufferHandle = subMesh.m_indexBufferHandle;
				renderPacket->m_indexCount = subMesh.m_indexCount;
				renderPacket->m_pixelShaderHandle = pixelShader->m_resourceHandle;

				// テクスチャの設定
				const kit::asset::TextureAsset* baseColorTextureAsset = nullptr;
				if (material->m_baseColorTextureHandle.IsValid())
				{
					baseColorTextureAsset = m_assetManager->FindTexture(material->m_baseColorTextureHandle);
				}

				const kit::asset::TextureAsset* metallicRoughnessTextureAsset = nullptr;
				if (material->m_metallicRoughnessTextureHandle.IsValid())
				{
					metallicRoughnessTextureAsset = m_assetManager->FindTexture(material->m_metallicRoughnessTextureHandle);
				}

				const kit::asset::TextureAsset* normalTextureAsset = nullptr;
				if (material->m_normalTextureHandle.IsValid())
				{
					normalTextureAsset = m_assetManager->FindTexture(material->m_normalTextureHandle);
				}

				const kit::asset::TextureAsset* occlusionTextureAsset = nullptr;
				if (material->m_occlusionTextureHandle.IsValid())
				{
					occlusionTextureAsset = m_assetManager->FindTexture(material->m_occlusionTextureHandle);
				}

				const kit::asset::TextureAsset* emissiveTextureAsset = nullptr;
				if (material->m_emissiveTextureHandle.IsValid())
				{
					emissiveTextureAsset = m_assetManager->FindTexture(material->m_emissiveTextureHandle);
				}

				if (baseColorTextureAsset)
				{
					renderPacket->m_baseColorTextureHandle = baseColorTextureAsset->m_resourceHandle;
					renderPacket->m_baseColorSamplerHandle = material->m_baseColorTextureSamplerHandle;
				}

				if (metallicRoughnessTextureAsset)
				{
					renderPacket->m_metallicRoughnessTextureHandle = metallicRoughnessTextureAsset->m_resourceHandle;
					renderPacket->m_metallicRoughnessSamplerHandle = material->m_metallicRoughnessTextureSamplerHandle;
				}

				if (normalTextureAsset)
				{
					renderPacket->m_normalTextureHandle = normalTextureAsset->m_resourceHandle;
					renderPacket->m_normalSamplerHandle = material->m_normalTextureSamplerHandle;
				}

				if (occlusionTextureAsset)
				{
					renderPacket->m_occlusionTextureHandle = occlusionTextureAsset->m_resourceHandle;
					renderPacket->m_occlusionSamplerHandle = material->m_occlusionTextureSamplerHandle;
				}

				if (emissiveTextureAsset)
				{
					renderPacket->m_emissiveTextureHandle = emissiveTextureAsset->m_resourceHandle;
					renderPacket->m_emissiveSamplerHandle = material->m_emissiveTextureSamplerHandle;
				}

				renderPacket->m_materialConstantBufferHandle = material->m_constantBufferHandle;
			}
		}

		return true;
	}
} // namespace ts

int32_t main()
{
	auto app = new ts::TransparentRenderingDemo();
	if (!app->Run())
	{
		TS_SAFE_RELEASE(app);

		return -1;
	}
	TS_SAFE_RELEASE(app);

	return 0;
}


