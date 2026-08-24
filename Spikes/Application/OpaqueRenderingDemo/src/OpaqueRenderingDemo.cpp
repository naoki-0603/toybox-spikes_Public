// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "OpaqueRenderingDemo.hpp"

#include <Core/Handle.hpp>

#include <Asset/ShaderAsset.hpp>

#include <Graphics/RenderPass/PresentationSetupRenderPass.hpp>
#include <Graphics/RenderPass/OpaqueRenderPass.hpp>

#include <Graphics/RenderResourceManager.hpp>

#include <Graphics/RHI/RHI_ShaderBindingsLayout.hpp>
#include <Graphics/RHI/RHI_PipelineState.hpp>
#include <Graphics/RHI/RHI_Buffer.hpp>
#include <Graphics/RHI/RHI_TextureView.hpp>


#define LOAD_TEXTURE_MULTITHREAD (1)

using namespace ts::kit::thread;
using namespace ts::kit::asset;

struct TexturePayload final
{
	ts::kit::fs::path m_path{};
	ts::kit::TextureAssetHandle* m_outHandle{};
	ts::kit::asset::AssetManager* m_assetManager{};
	std::latch* m_startingGun{};
};

static void Payload(void* payload)
{
	auto data = reinterpret_cast<TexturePayload*>(payload);
	data->m_startingGun->wait();

	LoadTextureConfig config = {};
	config.m_path = data->m_path;

	(*data->m_outHandle) = data->m_assetManager->LoadTexture(config);
}

namespace ts
{
	OpaqueRenderingDemo::OpaqueRenderingDemo() :
		Application(
			"RendererApplication",
			IVector2(640, 480)
		),
		m_renderer(), m_assetManager(),
		m_defaultAssetBuilder(), 
		m_defaultRenderResourcesBuilder(),
		m_sponzaMeshHandles(),
		m_jobSystem(),
		m_sponzaTextureHandles(),
		m_opaqueRenderPackets(),
		m_maskRenderPackets(),
		m_transparentRenderPackets()
	{
	}

	bool OpaqueRenderingDemo::OnInitialize()
	{
		{
			m_renderer = new kit::graphics::Renderer;

			kit::graphics::RendererInitDesc desc = {};
			desc.m_windowSize = m_window.GetSize();
			desc.m_windowHandle = m_window.GetHandle();
			desc.m_windowResizeEventDispatcher = m_window.GetResizeEventDispatcher();
			desc.m_engineTerminateEventDispatcher = &m_terminateEventDispatcher;
			desc.m_threadManager = &m_threadManager;

			if (!m_renderer->Create(desc))
			{
				return false;
			}
		}

		{
			kit::asset::AssetManagerInitDesc desc = {};
			desc.m_renderResourceManager = m_renderer->GetRenderResourceManager();
			desc.m_shaderCompiler = m_renderer->GetShaderCompiler();

			m_assetManager = new kit::asset::AssetManager;
			if (!m_assetManager->Create(desc))
			{
				return false;
			}
		}

		{
			if (!m_threadManager.RegisterWorkersFunc(
				[](void* jobSystem) -> void
				{
					auto system = reinterpret_cast<JobSystem*>(jobSystem);
					system->Work();
				},
				&m_jobSystem,
				kit::thread::WorkerType::Common
			))
			{
				return false;
			}
		}

		{
			m_defaultAssetBuilder = new kit::asset::DefaultAssetsBuilder;
			if (!m_defaultAssetBuilder->Build(m_assetManager))
			{
				return false;
			}
		}

		{
			kit::graphics::DefaultRenderResourcesBuilderDesc builderDesc = {};
			builderDesc.m_renderResourceManager = m_renderer->GetRenderResourceManager();
			builderDesc.m_staticMeshVertexShaderHandle = m_defaultAssetBuilder->GetStaticMeshVertexShaderHandle();
			builderDesc.m_meshPBRPixelShaderHandle = m_defaultAssetBuilder->GetMeshPBRPixelShaderHandle();
			builderDesc.m_meshPBRWithMaskPixelShaderHandle = m_defaultAssetBuilder->GetMeshPBRWithMaskPixelShaderHandle();

			m_defaultRenderResourcesBuilder = new kit::graphics::DefaultRenderResourcesBuilder;
			if (!m_defaultRenderResourcesBuilder->Build(builderDesc))
			{
				return false;
			}
		}

		LoadResources();
		CreateRenderPasses();
		CreateRenderPackets();

		return true;
	}

	bool OpaqueRenderingDemo::OnFinalize()
	{
		TS_SAFE_RELEASE(m_defaultRenderResourcesBuilder);
		TS_SAFE_RELEASE(m_defaultAssetBuilder);

		bool succeeded = true;
		if (m_assetManager)
		{
			succeeded = m_assetManager->Destroy();

			delete m_assetManager;
			m_assetManager = nullptr;
		}

		if (m_renderer)
		{
			succeeded = m_renderer->Destroy();

			delete m_renderer;
			m_renderer = nullptr;
		}

		return succeeded;
	}

	void OpaqueRenderingDemo::Update(const kit::ApplicationContext& context)
	{
		Application::Update(context);

		// シーン定数を更新する
		{
			m_sceneConstants.m_cameraPosition = Vector4(m_camera->GetEye(), 1.0f);
			m_sceneConstants.m_viewProjection = m_camera->GetViewProjection();
			m_sceneConstants.m_deltaTime = m_timer.GetDeltaTime();
			m_sceneConstants.m_totalTime = m_timer.GetTime();
		}

		// ウィンドウタイトルを更新する
		{
			std::string title = "OpaqueRenderingDemo FPS:";
			title += std::to_string(m_timer.GetFPS());

			m_window.SetWindowTitle(title);
		}
	}

	void OpaqueRenderingDemo::Render(const kit::ApplicationContext& context)
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

	void OpaqueRenderingDemo::LoadResources()
	{
		const kit::fs::path k_rootDirectory = kit::fs::GetRootDirectory();

		// Sponza textures
		{
			std::vector<kit::fs::path> paths = {};

			for (auto&& file : kit::fs::recursive_directory_iterator(kit::fs::path(k_rootDirectory / "Asset/Mesh/main_sponza/textures/")))
			{
				paths.emplace_back(file.path().lexically_normal());
			}
#if LOAD_TEXTURE_MULTITHREAD
			{

				std::latch waitLatch(paths.size());
				std::latch startingGun(1);

				std::vector<TexturePayload> m_payloads{};
				m_payloads.resize(paths.size());

				m_sponzaTextureHandles.resize(paths.size());
				for (u64 i = 0u; i < paths.size(); ++i)
				{
					m_payloads[i].m_assetManager = m_assetManager;
					m_payloads[i].m_path = paths[i];
					m_payloads[i].m_outHandle = &m_sponzaTextureHandles[i];
					m_payloads[i].m_startingGun = &startingGun;

					kit::thread::Job job = {};
					job.m_jobData = &m_payloads[i];
					job.m_latch = &waitLatch;
					job.m_execute = Payload;

					m_jobSystem.Submit(job, {}, kit::thread::JobPriority::High);
				}

				{
					ZoneScoped;

					// 一斉にロードを始める
					startingGun.count_down();

					// 開始時間の計測
					auto startTime = std::chrono::high_resolution_clock::now();

					waitLatch.wait();
				}
				
			}
#else // LOAD_TEXTURE_MULTITHREAD
			m_sponzaTextureHandles.resize(paths.size());

			// 開始時間の計測
			auto startTime = std::chrono::high_resolution_clock::now();

			for (u64 i = 0u; i < paths.size(); ++i)
			{
				LoadTextureConfig config = {};
				config.m_path = paths[i];

				m_sponzaTextureHandles[i] = m_assetManager->LoadTexture(config);
			}

			// 終了時間の計測
			auto finishTime = std::chrono::high_resolution_clock::now();

			auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();

			TS_INFO_LOG("Load Completed. Elapsed: {} ms", deltaTime);
#endif //! LOAD_TEXTURE_MULTITHREAD
		}

		// Sponza
		{
			LoadMeshConfig config = {};
			config.m_path = kit::fs::path(k_rootDirectory / "Asset/Mesh/main_sponza/NewSponza_Main_glTF_003.gltf").lexically_normal();
			config.m_staticMeshVertexShaderHandle = m_defaultAssetBuilder->GetStaticMeshVertexShaderHandle();
			config.m_meshPBRPixelShaderHandle = m_defaultAssetBuilder->GetMeshPBRPixelShaderHandle();

			m_sponzaMeshHandles = m_assetManager->LoadStaticMesh(config);
		}
	}

	void OpaqueRenderingDemo::CreateRenderPasses()
	{
		using namespace ts::kit::graphics;

		{
			RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 0u;

			auto presentationSetupRenderPass = new PresentationSetupRenderPass();

			if (!presentationSetupRenderPass->Create(renderPassDesc))
			{
				TS_SAFE_RELEASE(presentationSetupRenderPass);

				TS_FATAL_LOG("PresentationSetupRenderPassの作成に失敗しました。");
			}

			m_renderer->AddRenderPass(presentationSetupRenderPass);
		}

		{
			RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 1u;

			auto opaqueRenderPass = new OpaqueRenderPass(
				m_defaultRenderResourcesBuilder->GetOpaquePipelineStateHandle()
			);
			if (!opaqueRenderPass->Create(renderPassDesc))
			{
				TS_SAFE_RELEASE(opaqueRenderPass);
				TS_FATAL_LOG("OpaqueRenderPassの作成に失敗しました。");
			}

			m_renderer->AddRenderPass(opaqueRenderPass);
		}
	}

	void OpaqueRenderingDemo::CreateRenderPackets()
	{
		using namespace kit;
		using namespace kit::graphics;

		for (u64 meshIndex = 0u; meshIndex < m_sponzaMeshHandles.size(); ++meshIndex)
		{
			const kit::asset::StaticMeshAsset* mesh = m_assetManager->FindStaticMesh(
				m_sponzaMeshHandles[meshIndex]
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
	}
}
 
int32_t main()
{
	auto app = new ts::OpaqueRenderingDemo();
	if (!app->Run())
	{
		TS_SAFE_RELEASE(app);

		return -1;
	}
	TS_SAFE_RELEASE(app);

	return 0;
}
