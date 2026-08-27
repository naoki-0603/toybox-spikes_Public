// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_APPLICATION_APPLICATION_HPP_
#define SPIKES_KIT_COMMON_APPLICATION_APPLICATION_HPP_

#include "Core/ApplicationContext.hpp"
#include "Core/Timer.hpp"
#include "Core/InputSystem.hpp"
#include "Core/Window.hpp"
#include "Core/Thread/ThreadManager.hpp"

#include "Graphics/Camera.hpp"

namespace ts
{
	namespace kit
	{
		namespace event
		{
			struct EventWindowResize;
		} // namespace event

		/**
		 * @brief アプリケーションの基底クラス
		 * @details 
		 * 　独自の初期化/終了化を入れたい場合は、OnInitialize,OnFinalizeを上書きする
		 * 　アプリケーションから抜け出す条件はShouldCloseを上書きする
		 */
		class Application
		{
		public:
			Application(std::string_view name, const IVector2& size);
			virtual ~Application() noexcept = default;

			virtual bool Run();
		protected:
			virtual bool OnInitialize() = 0;
			virtual void Tick(const ApplicationContext& context);

			virtual void Update(const ApplicationContext& context);
			virtual void Render(const ApplicationContext& context);
			virtual bool OnFinalize() = 0;

			virtual bool ShouldClose() const noexcept;
		protected:
			virtual void OnWindowResize(const kit::event::EventWindowResize* data);
			virtual void OnWindowMouseScrolled(f32 wheelSpeed);

		protected:
			[[nodiscard]]
			const ApplicationContext& GetContext() const noexcept;

		protected:
			std::string m_name;
			IVector2 m_size;

			Timer m_timer;
			InputSystem m_inputSystem;
			Window m_window;
			Unique<graphics::PerspectiveCamera> m_camera;

			thread::ThreadManager m_threadManager;

			event::EventDispatcher m_terminateEventDispatcher;

			ApplicationContext m_context;
		private:
			[[nodiscard]]
			bool Initialize();

			[[nodiscard]]
			bool Finalize();
		};
	} // namespace kit
} // namespace ts

#endif // !SPIKES_KIT_COMMON_APPLICATION_APPLICATION_HPP_