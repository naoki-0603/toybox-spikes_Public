// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Application.hpp"
#include "Core/Thread/ThreadManager.hpp"
#include "Core/Event/CoreEvent.hpp"
#include "Core/Event/WindowEvent.hpp"

#include "Graphics/CameraController.hpp"

namespace ts
{
	namespace kit
	{
		Application::Application(std::string_view name, const IVector2& size) :
			m_name(name),
			m_size(size),
			m_timer(),
			m_inputSystem(),
			m_window(),
			m_camera(),
			m_threadManager(),
			m_terminateEventDispatcher(),
			m_context(
				m_timer,
				m_inputSystem,
				m_window
			)
		{
		}

		bool Application::Run()
		{
			if (!Initialize())
			{
				return false;
			}

			if (!OnInitialize())
			{
				return false;
			}

			while (!ShouldClose())
			{
				FrameMark;

				m_timer.Begin();
				m_inputSystem.OnUpdate(m_context);

				Tick(m_context);

				m_timer.End();
			}

			if (!Finalize())
			{
				return false;
			}

			return OnFinalize();
		}

		void Application::Tick(const ApplicationContext& context)
		{
			Update(context);
			Render(context);
		}
		
		void Application::Update(const ApplicationContext& context)
		{
			if (m_camera)
			{
				m_camera->OnUpdate(context);
			}
		}

		void Application::Render(const ApplicationContext& context)
		{
			(void)context;
		}

		bool Application::ShouldClose() const noexcept
		{
			return !(m_window.PollEvents());
		}

		void Application::OnWindowResize(const kit::event::EventWindowResize* data)
		{
			if (m_camera)
			{
				m_camera->OnWindowResize(data->m_size);
			}
		}

		void Application::OnWindowMouseScrolled(f32 wheelSpeed)
		{
			m_inputSystem.OnWindowMouseScrolled(wheelSpeed);
		}

		const ApplicationContext& Application::GetContext() const noexcept
		{
			return m_context;
		}

		bool Application::Initialize()
		{
			if (!m_threadManager.Create())
			{
				return false;
			}

			if (!m_window.Create(m_name, m_size))
			{
				return false;
			}

			m_inputSystem.Initialize();

			// カメラ
			{
				m_camera = std::make_unique<graphics::PerspectiveCamera>(
					Vector3(0.0f, 0.0f, -5.0f),
					Vector3::Zero(),
					kit::math::ToRadians(45.0f),
					static_cast<f32>(m_size.x) / static_cast<f32>(m_size.y),
					0.1f,
					2000.0f
				);

				m_camera->SetController(
					std::make_shared<graphics::EditorCameraController>()
				);
			}

			{
				m_window.GetResizeEventDispatcher()->RegisterListener(
					TS_BIND_EVENT(
						this,
						kit::Application,
						OnWindowResize,
						kit::event::EventWindowResize,
						0
					)
				);
			}

			return true;
		}

		bool Application::Finalize()
		{
			event::EventEngineTerminate eventData = {};
			m_terminateEventDispatcher.Dispatch(&eventData);

			if (!m_threadManager.Destroy())
			{
				return false;
			}

			m_inputSystem.Finalize();

			return true;
		}
	} // namespace kit
} // namespace ts