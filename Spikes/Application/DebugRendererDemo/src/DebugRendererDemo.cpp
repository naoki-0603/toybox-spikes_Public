// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "DebugRendererDemo.hpp"

#include <Graphics/Renderer.hpp>
#include <Graphics/Debug/DebugRenderer.hpp>
#include <Graphics/Debug/DebugRenderPass.hpp>

#include <Graphics/RenderPass/PresentationSetupRenderPass.hpp>

#include <Graphics/Builder/DefaultRenderResourcesBuilder.hpp>

#include <Asset/AssetManager.hpp>
#include <Asset/Builder/DefaultAssetsBuilder.hpp>

#include <Core/Thread/JobSystem.hpp>

namespace ts
{
	DebugRendererDemo::DebugRendererDemo() :
		Application("DebugRendererDemo", IVector2(640, 480)),
		m_renderer(),
		m_debugRenderer(),
		m_defaultRenderResourceBuilder(),
		m_assetManager(),
		m_defaultAssetsBuilder(),
		m_jobSystem(),
		m_opaqueRenderPackets(),
		m_maskRenderPackets(),
		m_transparentRenderPackets(),
		m_sceneConstants()
	{
	}

	bool DebugRendererDemo::OnInitialize()
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

		// デバッグレンダラーの作成
		{
			m_debugRenderer = new kit::graphics::DebugRenderer;
			if (!m_debugRenderer->Create(m_renderer->GetRenderResourceManager()))
			{
				TS_FATAL_LOG("DebugRendererの作成に失敗しました。");

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

			const kit::asset::ShaderAsset* debugLineVertexShader = m_assetManager->FindShader(
				m_defaultAssetsBuilder->GetDebugLineVertexShaderHandle()
			);

			const kit::asset::ShaderAsset* debugLinePixelShader = m_assetManager->FindShader(
				m_defaultAssetsBuilder->GetDebugLinePixelShaderHandle()
			);

			m_defaultRenderResourceBuilder = new kit::graphics::DefaultRenderResourcesBuilder;

			kit::graphics::DefaultRenderResourcesBuilderDesc desc = {};
			desc.m_renderResourceManager = m_renderer->GetRenderResourceManager();
			desc.m_staticMeshVertexShaderHandle = staticMeshVertexShader->m_resourceHandle;
			desc.m_meshPBRPixelShaderHandle = meshPBRPixelShader->m_resourceHandle;
			desc.m_meshPBRWithMaskPixelShaderHandle = meshPBRWithMaskPixelShader->m_resourceHandle;
			desc.m_debugLineVertexShaderHandle = debugLineVertexShader->m_resourceHandle;
			desc.m_debugLinePixelShaderHandle = debugLinePixelShader->m_resourceHandle;
			
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

		if (!CreateRenderPasses())
		{
			TS_FATAL_LOG("RenderPassの作成に失敗しました。");

			return false;
		}

		return true;
	}

	bool DebugRendererDemo::OnFinalize()
	{
		TS_SAFE_RELEASE(m_jobSystem);

		if (!m_renderer->Destroy())
		{
			TS_FATAL_LOG("Rendererの破棄に失敗しました。");

			return false;
		}

		if (!m_debugRenderer->Destroy())
		{
			TS_FATAL_LOG("DebugRendererの破棄に失敗しました。");

			return false;
		}

		TS_SAFE_RELEASE(m_debugRenderer);
		TS_SAFE_RELEASE(m_renderer);
		
		return true;
	}

	void DebugRendererDemo::Update(const kit::ApplicationContext& context)
	{
		Application::Update(context);

		m_debugRenderer->Begin();

		// DebugLineParameters
		{
			kit::graphics::DebugLineParameters lineParameters = {};
			lineParameters.m_begin = Vector3(-4.0f, 0.0f, 1.0f);
			lineParameters.m_beginColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			lineParameters.m_end = Vector3(-3.0f, 0.0f, 1.0f);
			lineParameters.m_endColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawLine(lineParameters);
		}

		// DebugCircle
		{
			kit::graphics::DebugCircleParameters circleParameters = {};
			circleParameters.m_center = { 0.0f, 0.0f, 1.0f };
			circleParameters.m_color  = { 1.0f, 0.0f, 0.0f, 1.0f };
			circleParameters.m_radius = 0.5f;

			m_debugRenderer->DrawCircle(circleParameters);
		}

		// DebugHalfCircle
		{
			kit::graphics::DebugHalfCircleParameters halfCircleParameters = {};
			halfCircleParameters.m_center = { 2.0f, 0.0f, 1.0f };
			halfCircleParameters.m_color = { 1.0f, 0.0f, 0.0f, 1.0f };
			halfCircleParameters.m_radius = 0.5f;

			m_debugRenderer->DrawHalfCircle(halfCircleParameters);
		}

		// DebugBox
		{
			kit::graphics::DebugBoxParameters boxParameters = {};
			boxParameters.m_min = { -2.0f, -0.5f, +1.0f };
			boxParameters.m_max = { -1.0f, +0.5f, +2.0f };
			boxParameters.m_color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawBox(boxParameters);
		}

		// DebugSphere
		{
			kit::graphics::DebugSphereParameters sphereParameters = {};
			sphereParameters.m_center = { 4.0f, 0.0f, 1.0f };
			sphereParameters.m_radius = 0.5f;
			sphereParameters.m_color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawSphere(sphereParameters);
		}

		// DebugCone
		{
			kit::graphics::DebugConeParameters coneParameters = {};
			coneParameters.m_apex = { 6.0f, 0.0f, 1.0f };
			coneParameters.m_height = -1.0f;
			coneParameters.m_radius = 0.5f;
			coneParameters.m_color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawCone(coneParameters);

			coneParameters.m_apex = { 6.0f, 0.0f, 1.0f };
			coneParameters.m_height = -1.0f;
			coneParameters.m_radius = 0.5f;
			coneParameters.m_color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			coneParameters.m_rotation = Quaternion::RotationAxis(
				Vector3::Right(), kit::math::k_pidiv2
			);

			m_debugRenderer->DrawCone(coneParameters);
		}

		// DebugCapsule
		{
			kit::graphics::DebugCapsuleParameters capsuleParameters = {};
			capsuleParameters.m_center = { 8.0f, 0.0f, 1.0f };
			capsuleParameters.m_cylinderHeight = 1.0f;
			capsuleParameters.m_radius = 0.5f;
			capsuleParameters.m_color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawCapsule(capsuleParameters);

			capsuleParameters.m_center = { 8.0f, 1.5f, 1.0f };
			capsuleParameters.m_cylinderHeight = 1.0f;
			capsuleParameters.m_radius = 0.5f;
			capsuleParameters.m_color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			capsuleParameters.m_rotation = Quaternion::RotationAxis(
				Vector3::Right(), kit::math::k_pidiv2
			);

			m_debugRenderer->DrawCapsule(capsuleParameters);
		}

		// DebugArrow
		{
			kit::graphics::DebugArrowParameters arrowParameters = {};
			arrowParameters.m_start = { 10.0f, 0.0f, 1.0f };
			arrowParameters.m_direction = Vector3::Right();
			arrowParameters.m_color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawArrow(arrowParameters);

			arrowParameters.m_start = { 10.0f, 0.0f, 1.0f };
			arrowParameters.m_direction = Vector3::Up();
			arrowParameters.m_color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

			m_debugRenderer->DrawArrow(arrowParameters);

			arrowParameters.m_start = { 10.0f, 0.0f, 1.0f };
			arrowParameters.m_direction = Vector3::Front();
			arrowParameters.m_color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

			m_debugRenderer->DrawArrow(arrowParameters);
		}
	
		// 定数の更新
		{
			m_sceneConstants.m_cameraPosition = Vector4(m_camera->GetEye(), 1.0f);
			m_sceneConstants.m_viewProjection = m_camera->GetViewProjection();
			m_sceneConstants.m_deltaTime = context.m_timerRef.GetDeltaTime();
			m_sceneConstants.m_totalTime = context.m_timerRef.GetTime();
		}

		// ウィンドウタイトルの更新
		{
			const std::string title = "DebugRendererDemo (FPS: " + 
				std::to_string(context.m_timerRef.GetFPS()) + ")";

			m_window.SetWindowTitle(title);
		}

		m_debugRenderer->End(m_renderer->GetRenderResourceManager());
	}

	void DebugRendererDemo::Render(const kit::ApplicationContext& context)
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

	bool DebugRendererDemo::CreateRenderPasses()
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

		// DebugRenderPass
		{
			kit::graphics::RenderPassDesc renderPassDesc = {};
			renderPassDesc.m_passIndex = 1u;

			auto debugRenderPass = new kit::graphics::DebugRenderPass(
				m_debugRenderer->GetVertexBufferHandle(),
				m_defaultRenderResourceBuilder->GetLinePipelineStateHandle()
			);
			if (!debugRenderPass->Create(renderPassDesc))
			{
				return false;
			}

			m_renderer->AddRenderPass(debugRenderPass);
		}

		return true;
	}
} // namespace ts

int32_t main()
{
	auto app = new ts::DebugRendererDemo();
	if (!app->Run())
	{
		TS_SAFE_RELEASE(app);

		return -1;
	}
	TS_SAFE_RELEASE(app);

	return 0;
}


