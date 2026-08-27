// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Graphics/Renderer.hpp"

#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/RenderPipeline/ForwardRenderPipeline.hpp"

#include "Core/Event/CoreEvent.hpp"
#include "Core/Event/WindowEvent.hpp"
#include "Core/Thread/ThreadManager.hpp"

#include "Graphics/RHI/RHI_Device.hpp"
#include "Graphics/RHI/RHI_CommandQueue.hpp"
#include "Graphics/RHI/RHI_ShaderCompiler.hpp"

#include "Graphics/RenderResourceManager.hpp"

#include "Graphics/ObjectConstants.hpp"

namespace ts
{
	namespace kit
	{
		namespace fs
		{
			[[nodiscard]]
			path GetProjectFullPath()
			{
#ifdef TS_PROJECT_ROOT
				return path(TS_PROJECT_ROOT);
#else // TS_PROJECT_ROOT
#error "TS_PROJECT_ROOT is not defined in CMakeLists.txt!"
				return path("");
#endif //! TS_PROJECT_ROOT
			}
		} // namespace

		namespace graphics
		{
			static void RenderJobSystemWork(void* jobSystem)
			{
				auto renderJobSystem = reinterpret_cast<thread::RenderJobSystem*>(jobSystem);

				renderJobSystem->Work();
			}

			// DLL RHI生成関数
			using CreateRHIPhyisicalDeviceFunc = RHI_PhyisicalDevice * (*)();
			using CreateRHIDeviceFunc = RHI_Device * (*)();
			using CreateRHIShaderCompilerFunc = RHI_ShaderCompiler * (*)();

			Renderer::Renderer() :
				m_device(),
				m_phyisicalDevice(),
				m_shaderCompiler(),
				m_swapChain(),
				m_graphicsCommandQueue(),
				m_frameAllocator(),
				m_commandBuffers(),
				m_commandRecorders(),
				m_scratchBuffers(),
				m_resourceRecorders(),
				m_renderPipeline(),
				m_context(),
				m_renderJobSystem(),
				m_sceneConstantsBufferHandle(),
				m_objectConstantsBufferHandle(),
				m_renderResourceManager()
			{
			}

			bool Renderer::Create(const RendererInitDesc& desc)
			{
				if (!desc.m_windowHandle)
				{
					TS_ASSERT(false, "WindowHandleがnullptrです。");

					return false;
				}

				if (!desc.m_windowResizeEventDispatcher)
				{
					TS_ASSERT(false, "WindowResizeEventDispatcherがnullptrです。");

					return false;
				}

				if (!desc.m_threadManager)
				{
					TS_ASSERT(false, "ThreadManagerがnullptrです。");

					return false;
				}

				if (!desc.m_engineTerminateEventDispatcher)
				{
					TS_ASSERT(false, "EngineTerminateEventDispatcherがnullptrです。");

					return false;
				}

				if (!CreateRHIObjects(desc))
				{
					TS_ASSERT(false, "RHIObjectsの生成に失敗しました。");

					return false;
				}

				{
					resource::RenderResourceManagerInitDesc desc = {};
					desc.m_device = m_device;

					m_renderResourceManager = new resource::RenderResourceManager;
					if (!m_renderResourceManager->Create(desc))
					{
						TS_ASSERT(false, "RenderResourceManagerの生成に失敗しました。");

						return false;
					}
				}

				// シーン定数バッファ
				{
					RHI_BufferDesc bufferDesc = {};
					bufferDesc.m_type = RHI_BufferType::ConstantBuffer;
					bufferDesc.m_usage = RHI_BufferUsage::Dynamic;
					bufferDesc.m_strideInBytes = sizeof(SceneConstants);
					bufferDesc.m_count = 1u;
					bufferDesc.m_data = nullptr;

					m_sceneConstantsBufferHandle = m_renderResourceManager->CreateBuffer(
						bufferDesc
					);
				}

				// オブジェクト定数バッファ
				{
					RHI_BufferDesc bufferDesc = {};
					bufferDesc.m_type = RHI_BufferType::ConstantBuffer;
					bufferDesc.m_usage = RHI_BufferUsage::Dynamic;
					bufferDesc.m_strideInBytes = sizeof(ObjectConstants);
					bufferDesc.m_count = 1u;
					bufferDesc.m_data = nullptr;

					m_objectConstantsBufferHandle = m_renderResourceManager->CreateBuffer(
						bufferDesc
					);
				}

				if (!m_frameAllocator.Create(k_commandBufferSizeBytes))
				{
					TS_ASSERT(false, "FrameAllocatorの生成に失敗しました。");

					return false;
				}

				// コマンドバッファ群の生成 仮で32パス分確保
				{
					m_commandBuffers.resize(32u);
					m_commandRecorders.resize(m_commandBuffers.size());

					m_scratchBuffers.resize(32u);
					m_resourceRecorders.resize(m_scratchBuffers.size());
				}

				// レンダーパイプラインの作成
				m_renderPipeline = std::make_unique<ForwardRenderPipeline>();
				if (!m_renderPipeline->Create(m_device, m_commandBuffers))
				{
					TS_ASSERT(false, "レンダーパイプラインの生成に失敗しました。");

					return false;
				}

				// リサイズコールバックを設定
				desc.m_windowResizeEventDispatcher->RegisterListener(
					TS_BIND_EVENT(
						this,
						Renderer,
						OnWindowResize,
						event::EventWindowResize,
						0
					)
				);

				// Terminateコールバックを設定
				desc.m_engineTerminateEventDispatcher->RegisterListener(
					TS_BIND_EVENT(
						&m_renderJobSystem,
						thread::RenderJobSystem,
						OnEngineTerminate,
						event::EventEngineTerminate,
						0u
					)
				);

				// スレッドの登録
				if (!desc.m_threadManager->RegisterWorkersFunc(
					RenderJobSystemWork,
					&m_renderJobSystem,
					thread::WorkerType::Render
				))
				{
					TS_ASSERT(false, "ワーカー関数の登録に失敗しました");

					return false;
				}

				return true;
			}

			bool Renderer::Destroy()
			{
				if (!m_renderPipeline->Destroy())
				{
					return false;
				}

				{
					if (!m_renderResourceManager->Destroy())
					{
						delete m_renderResourceManager;
						m_renderResourceManager = nullptr;

						return false;
					}

					delete m_renderResourceManager;
					m_renderResourceManager = nullptr;
				}

				if (!m_swapChain->Destroy())
				{
					return false;
				}

				if (!m_graphicsCommandQueue->Destroy())
				{
					return false;
				}

				if (!m_shaderCompiler->Destroy())
				{
					return false;
				}

				if (m_phyisicalDevice)
				{
					if (!m_phyisicalDevice->Destroy())
					{
						return false;
					}
				}

				if (m_device)
				{
					if (!m_device->Destroy())
					{
						return false;
					}
				}

				return true;
			}

			void Renderer::AddRenderPass(RenderPass* pass)
			{
				m_renderPipeline->AddRenderPass(pass);
			}

			void Renderer::BeginFrame(const SceneConstants& constants)
			{
				RHI_Buffer* sceneConstantBuffer = m_renderResourceManager->FindBuffer(
					m_sceneConstantsBufferHandle
				);
				sceneConstantBuffer->WriteData(
					&constants,
					sizeof(SceneConstants),
					0u
				);
			}

			void Renderer::Render(const RenderPacketsBundle& bundle)
			{
				{
					// Reset
					m_frameAllocator.Reset();

					// Allocate memory.
					for (u32 i = 0u; i < 16u; ++i)
					{
						m_commandBuffers[i] = RenderCommandBuffer(m_frameAllocator.Allocate(
							k_commandBufferEachPassSizeBytes
						));
						m_commandRecorders[i] = RenderCommandRecorder(&m_commandBuffers[i], i);

						m_scratchBuffers[i] = RenderScratchBuffer(m_frameAllocator.Allocate(
							k_scratchBufferEachPassSizeBytes
						));
						m_resourceRecorders[i] = RenderResourceRecorder(&m_scratchBuffers[i]);
					}
				}

				// Set Render resource manager
				m_context.m_renderResourceManager = m_renderResourceManager;

				m_context.m_mainViewport = &m_swapChain->GetViewport();

				// Set current swapchain rtv/dsv.
				m_context.m_swapChainCurrentRTV = m_swapChain->GetCurrentRTV();
				m_context.m_swapChainCurrentDSV = m_swapChain->GetCurrentDSV();

				m_context.m_sceneConstantsBufferHandle = m_sceneConstantsBufferHandle;
				m_context.m_objectConstantsBufferHandle = m_objectConstantsBufferHandle;

				{
					ZoneScopedNC("RenderPipeline", tracy::Color::Aqua);

					m_renderPipeline->Render(
						m_context,
						m_renderJobSystem,
						m_commandRecorders,
						m_resourceRecorders,
						bundle
					);
				}

				{
					ZoneScopedNC("GraphicsCommandQueue", tracy::Color::Green);

					m_graphicsCommandQueue->Execute(
						m_commandBuffers
					);
				}
			}

			void Renderer::EndFrame()
			{
			}

			void Renderer::Present()
			{
				if (!m_swapChain->Present())
				{
					TS_FATAL_LOG("SwapChainのPresentに失敗しました");
				}
			}

			void Renderer::OnWindowResize(const event::EventWindowResize* data)
			{
				(void)data;
			}

			bool Renderer::CreateRHIObjects(const RendererInitDesc& desc)
			{
				const fs::path projectFullPath = fs::GetProjectFullPath();

#ifdef TS_DEBUG || TS_RELEASE
				const fs::path moduleFullPath = projectFullPath / "Bin" / "Plugins" / "Debug" / "SpikesKit_DX11.dll";

				HMODULE rhiModule = LoadLibraryA(moduleFullPath.string().c_str());
				if (!rhiModule)
				{
					TS_FATAL_LOG("モジュール {} の読み込みに失敗しました。", moduleFullPath.string());

					return false;
				}

				auto createPhyisicalDeviceFunc = reinterpret_cast<CreateRHIPhyisicalDeviceFunc>(GetProcAddress(rhiModule, "CreateRHIPhyisicalDevice"));
				m_phyisicalDevice = createPhyisicalDeviceFunc();

				if (!m_phyisicalDevice->Create(PhyisicalDeviceOptions::EnableDebug))
				{
					return false;
				}

				auto createDeviceFunc = reinterpret_cast<CreateRHIDeviceFunc>(GetProcAddress(rhiModule, "CreateRHIDevice"));
				m_device = createDeviceFunc();

				if (!m_device->Create(m_phyisicalDevice, DeviceOptions::EnableDebug))
				{
					return false;
				}

#else // (TS_DEBUG || TS_RELEASE)
				const fs::path moduleFullPath = projectFullPath / "Bin" / "Plugins" / "Release" / "SpikesKit_DX11.dll";

				HMODULE rhiModule = LoadLibraryA(moduleFullPath.string().c_str());
				if (!rhiModule)
				{
					TS_FATAL_LOG("モジュール {} の読み込みに失敗しました。", moduleFullPath.string());

					return false;
				}

				auto createPhyisicalDeviceFunc = reinterpret_cast<CreateRHIPhyisicalDeviceFunc>(GetProcAddress(rhiModule, "CreateRHIPhyisicalDevice"));
				m_phyisicalDevice = createPhyisicalDeviceFunc();

				if (!m_phyisicalDevice->Create(PhyisicalDeviceOptions::Default))
				{
					return false;
				}

				auto createDeviceFunc = reinterpret_cast<CreateRHIDeviceFunc>(GetProcAddress(rhiModule, "CreateRHIDevice"));
				m_device = createDeviceFunc();

				if (!m_device->Create(m_phyisicalDevice, DeviceOptions::Default))
				{
					return false;
				}
#endif // !(TS_DEBUG || TS_RELEASE)

				// ShaderCompiler
				{
					auto createRHIShaderCompilerFunc = reinterpret_cast<CreateRHIShaderCompilerFunc>(
						GetProcAddress(rhiModule, "CreateRHIShaderCompiler")
						);
					m_shaderCompiler = createRHIShaderCompilerFunc();
				}

				// SwapChain
				{
					RHI_SwapChainDesc swapChainDesc = {};
					swapChainDesc.m_bufferCount = 2u;
					swapChainDesc.m_format = RHI_Format::R8G8B8A8_UNorm;
					swapChainDesc.m_size = desc.m_windowSize;
					swapChainDesc.m_windowHandle = desc.m_windowHandle;

					m_swapChain = m_device->CreateSwapChain(swapChainDesc);
				}
				m_graphicsCommandQueue = m_device->CreateCommandQueue(CommandQueueType::Graphics);

				return true;
			}
		} // namespace graphics
	} // namespace kit
} // namespace ts