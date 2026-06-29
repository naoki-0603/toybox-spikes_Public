// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#ifndef SPIKES_KIT_COMMON_CORE_INPUT_SYSTEM_HPP_
#define SPIKES_KIT_COMMON_CORE_INPUT_SYSTEM_HPP_

namespace ts
{
	namespace kit
	{
		struct ApplicationContext;

		enum class Key : i32
		{
			MouseLeft, MouseMiddle, MouseRight,
			MouseXButton1, MouseXButton2,

			KeyBackspace, KeyTab, KeyClear, KeyReturn,
			KeyEscape, KeySpace, KeyLeft, KeyUp,
			KeyRight, KeyDown, Key0, Key1,
			Key2, Key3, Key4, Key5, 
			Key6, Key7, Key8, Key9,
			KeyA, KeyB, KeyC, KeyD,
			KeyE, KeyF, KeyG, KeyH,
			KeyI, KeyJ, KeyK, KeyL,
			KeyM, KeyN, KeyO, KeyP,
			KeyQ, KeyR, KeyS, KeyT,
			KeyU, KeyV, KeyW, KeyX,
			KeyY, KeyZ, KeyF1, KeyF2,
			KeyF3, KeyF4, KeyF5, KeyF6,
			KeyF7, KeyF8, KeyF9, KeyF10,
			KeyF11, KeyF12, KeyLShift, KeyRShift,
			KeyLControl, KeyRControl,

			Max
		};

		class InputSystem final
		{
		public:
			InputSystem();
			InputSystem(const InputSystem&) = delete;
			InputSystem(InputSystem&&) noexcept = delete;
			~InputSystem() noexcept = default;

			InputSystem& operator=(const InputSystem&) = delete;
			InputSystem& operator=(InputSystem&&) noexcept = delete;
		public:
			void Initialize();
			void OnUpdate(const ApplicationContext& context);
			void Finalize();

			void OnWindowMouseScrolled(f32 wheelSpeed);
		public:
			[[nodiscard]]
			bool IsKeyPressed(Key key) const;

			[[nodiscard]]
			bool IsKeyTriggered(Key key) const;

			[[nodiscard]]
			bool IsKeyReleased(Key key) const;

			[[nodiscard]]
			const Vector2& GetMousePan() const noexcept { return m_mousePan; }

			[[nodiscard]]
			f32 GetMouseWheelSpeed() const noexcept { return m_mouseWheelSpeed; }
		private:
			[[nodiscard]]
			i32 KeyMapping(Key key) const;

		private:
			/// マウス
			IVector2 m_mousePreviousPosition;
			IVector2 m_mouseCurrentPosition;
			Vector2 m_mousePan;
			f32 m_mouseWheelSpeed;

			/// キーボード
			u8 m_keyboardPreviousData[256u];
			u8 m_keyboardCurrentData[256u];
		};
	} // namespace kit
} // namespace ts

#endif //! SPIKES_KIT_COMMON_CORE_INPUT_SYSTEM_HPP_