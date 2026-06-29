// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/InputSystem.hpp"
#include "Core/ApplicationContext.hpp"
#include "Core/Window.hpp"

namespace ts
{
	namespace kit
	{
		InputSystem::InputSystem() :
			m_mousePreviousPosition(),
			m_mouseCurrentPosition(),
			m_mousePan(),
			m_mouseWheelSpeed(),
			m_keyboardPreviousData(),
			m_keyboardCurrentData()
		{
		}

		void InputSystem::Initialize()
		{
		}

		void InputSystem::OnUpdate(const ApplicationContext& context)
		{
			// Mouse
			{
				m_mouseWheelSpeed = 0.0f;

				// マウスの位置を更新
				m_mousePreviousPosition = m_mouseCurrentPosition;

				POINT point;
				GetCursorPos(&point);

				ScreenToClient(
					context.m_windowRef.GetHandle(),
					&point
				);

				m_mouseCurrentPosition = IVector2(point.x, point.y);

				// マウスのパンを計算
				constexpr f32 k_maxPan = 10.0f;
				IVector2 mousePan = m_mouseCurrentPosition - m_mousePreviousPosition;
				m_mousePan.x = static_cast<f32>(mousePan.x);
				m_mousePan.y = static_cast<f32>(mousePan.y);

				// X座標
				bool isMinusX = (m_mousePan.x < 0.0f);
				if (std::fabsf(m_mousePan.x) > k_maxPan)
				{
					m_mousePan.x = k_maxPan * (isMinusX ? -1.0f : 1.0f);
				}
				
				// Y座標
				bool isMinusY = (m_mousePan.y < 0.0f);
				if (std::fabsf(m_mousePan.y) > k_maxPan)
				{
					m_mousePan.y = k_maxPan * (isMinusY ? -1.0f : 1.0f);
				}
			}

			// Keyboard
			{
				std::memcpy(m_keyboardPreviousData, m_keyboardCurrentData, 256);

				(void)GetKeyboardState(m_keyboardCurrentData);
			}
		}

		void InputSystem::Finalize()
		{
		}

		void InputSystem::OnWindowMouseScrolled(f32 wheelSpeed)
		{
			m_mouseWheelSpeed = wheelSpeed;
		}

		bool InputSystem::IsKeyPressed(Key key) const
		{
			const i32 mappedKey = KeyMapping(key);
			if (mappedKey == -1)
			{
				return false;
			}

			return (m_keyboardCurrentData[mappedKey] & 0x80u);
		}

		bool InputSystem::IsKeyTriggered(Key key) const
		{
			const i32 mappedKey = KeyMapping(key);
			if (mappedKey == -1)
			{
				return false;
			}

			return (~(m_keyboardPreviousData[mappedKey]) & (m_keyboardCurrentData[mappedKey]));
		}

		bool InputSystem::IsKeyReleased(Key key) const
		{
			const i32 mappedKey = KeyMapping(key);
			if (mappedKey == -1)
			{
				return false;
			}

			return ((m_keyboardPreviousData[mappedKey] & 0x80u) && !(m_keyboardCurrentData[mappedKey] & 0x80u));
		}

		i32 InputSystem::KeyMapping(Key key) const
		{
			switch (key)
			{
				case Key::MouseLeft: return VK_LBUTTON;
				case Key::MouseMiddle: return VK_MBUTTON;
				case Key::MouseRight: return VK_RBUTTON;
				case Key::MouseXButton1: return VK_XBUTTON1;
				case Key::MouseXButton2: return VK_XBUTTON2;
				case Key::KeyBackspace: return VK_BACK;
				case Key::KeyTab: return VK_TAB;
				case Key::KeyClear: return VK_CLEAR;
				case Key::KeyReturn: return VK_RETURN;
				case Key::KeyEscape: return VK_ESCAPE;
				case Key::KeySpace: return VK_SPACE;
				case Key::KeyLeft: return VK_LEFT;
				case Key::KeyUp: return VK_UP;
				case Key::KeyRight: return VK_RIGHT;
				case Key::KeyDown: return VK_DOWN;
				case Key::Key0: return '0';
				case Key::Key1: return '1';
				case Key::Key2: return '2';
				case Key::Key3: return '3';
				case Key::Key4: return '4';
				case Key::Key5: return '5';
				case Key::Key6: return '6';
				case Key::Key7: return '7';
				case Key::Key8: return '8';
				case Key::Key9: return '9';
				case Key::KeyA: return 'A';
				case Key::KeyB: return 'B';
				case Key::KeyC: return 'C';
				case Key::KeyD: return 'D';
				case Key::KeyE: return 'E';
				case Key::KeyF: return 'F';
				case Key::KeyG: return 'G';
				case Key::KeyH: return 'H';
				case Key::KeyI: return 'I';
				case Key::KeyJ: return 'J';
				case Key::KeyK: return 'K';
				case Key::KeyL: return 'L';
				case Key::KeyM: return 'M';
				case Key::KeyN: return 'N';
				case Key::KeyO: return 'O';
				case Key::KeyP: return 'P';
				case Key::KeyQ: return 'Q';
				case Key::KeyR: return 'R';
				case Key::KeyS: return 'S';
				case Key::KeyT: return 'T';
				case Key::KeyU: return 'U';
				case Key::KeyV: return 'V';
				case Key::KeyW: return 'W';
				case Key::KeyX: return 'X';
				case Key::KeyY: return 'Y';
				case Key::KeyZ: return 'Z';
				case Key::KeyF1: return VK_F1;
				case Key::KeyF2: return VK_F2;
				case Key::KeyF3: return VK_F3;
				case Key::KeyF4: return VK_F4;
				case Key::KeyF5: return VK_F5;
				case Key::KeyF6: return VK_F6;
				case Key::KeyF7: return VK_F7;
				case Key::KeyF8: return VK_F8;
				case Key::KeyF9: return VK_F9;
				case Key::KeyF10: return VK_F10;
				case Key::KeyF11: return VK_F11;
				case Key::KeyF12: return VK_F12;
				case Key::KeyLShift: return VK_F12;
				case Key::KeyRShift: return VK_F12;
				case Key::KeyLControl: return VK_F12;
				case Key::KeyRControl: return VK_F12;
			}

			TS_ASSERT(false, "不明なキータイプです。");

			return -1;
		}
	} // namespace kit
} // namespace ts