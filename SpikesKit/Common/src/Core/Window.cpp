// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/Window.hpp"

#include "Core/Event/WindowEvent.hpp"

#include <backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ts
{
	namespace kit
	{
		namespace internal
		{
			LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
				if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
				{
					return static_cast<LRESULT>(true);
				}

				auto self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

				if (self)
				{
					switch (msg)
					{
					case WM_SIZE:
					{
						const int32_t width = LOWORD(lParam);
						const int32_t height = HIWORD(lParam);

						self->Resize(IVector2(width, height));
					}
					break;
					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						HDC hdc = BeginPaint(hwnd, &ps);
						FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
						EndPaint(hwnd, &ps);
					}
					break;
					// ウィンドウが閉じられる
					case WM_CLOSE:
					{
						// DefWindowProc内で DestroyWindowが呼ばれるため早期リターン
						return self->Destroy() ? 0 : -1;
					}
					break;
					// ウィンドウが破棄された
					case WM_DESTROY:
					{
						PostQuitMessage(0);

						// WM_CLOSEと同じ理由
						return 0;
					}
					break;
					case WM_MOUSEWHEEL:
					{
						const auto wheelSpeed = static_cast<f32>(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);

					}
					break;
					}
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		} // namespace internal

		Window::Window() : 
			m_name(),
			m_size(),
			m_handle(),
			m_resizeEventDispatcher()
		{
		}

		bool Window::Create(std::string_view name, const IVector2& size)
		{
			m_name = name;
			m_size = size;

			// ウィンドウ情報を登録する
			WNDCLASSEX classSpec = {};
			{
				classSpec.cbSize = sizeof(WNDCLASSEX);

				// TODO(naoki): ユーザーがカスタマイズできるようにする
				classSpec.style = CS_VREDRAW | CS_HREDRAW;

				classSpec.hInstance = GetModuleHandleA(nullptr);
				classSpec.lpfnWndProc = internal::WindowProc;
				classSpec.hCursor = LoadCursorA(classSpec.hInstance, IDC_ARROW);
				classSpec.cbClsExtra = 0;
				classSpec.cbWndExtra = 0;
				classSpec.hbrBackground = nullptr;
				classSpec.lpszClassName = m_name.c_str();
				classSpec.lpszMenuName = nullptr;
				classSpec.hIcon = nullptr;
				classSpec.hIconSm = nullptr;

				RegisterClassEx(&classSpec);

				// ウィンドウの作成を行う
				{
					m_handle = CreateWindowEx(
						0u,
						m_name.c_str(),
						m_name.c_str(),
						WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						m_size.x,
						m_size.y,
						nullptr,
						nullptr,
						classSpec.hInstance,
						nullptr
					);
				}

				// 自身のポインターをユーザーポインタとして設定する
				SetWindowLongPtr(m_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

				// ウィンドウのサイズを設定する
				SetSize(m_size);

				// ウィンドウを表示する
				ShowWindow(m_handle, SW_SHOW);
			}
			
			return true;
		}

		bool Window::PollEvents() const noexcept
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					return false;
				}

				TranslateMessage(&msg);

				DispatchMessage(&msg);
			}

			return true;
		}

		bool Window::Destroy()
		{
			::DestroyWindow(m_handle);

			return true;
		}

		void Window::SetSize(const IVector2& size)
		{
			RECT wr;
			GetWindowRect(m_handle, &wr);

			RECT wc;
			GetClientRect(m_handle, &wc);

			const int32_t newWidth = (wr.right - wr.left) - (wc.right - wc.left) + size.x;
			const int32_t newHeight = (wr.bottom - wr.top) - (wc.bottom - wc.top) + size.y;

			// ウィンドウのサイズを設定する
			SetWindowPos(
				m_handle,
				nullptr,
				0,
				0,
				newWidth,
				newHeight,
				SWP_NOMOVE | SWP_NOZORDER
			);
		}

		void Window::SetWindowTitle(const std::string& title)
		{
			SetWindowTextA(
				m_handle,
				title.c_str()
			);
		}

		void Window::Resize(const IVector2& size)
		{
			SetSize(size);

			event::EventWindowResize resizeEvent = {};
			resizeEvent.m_size = size;

			m_resizeEventDispatcher.Dispatch(&resizeEvent);
		}
	} // namespace kit
} // namespace sp

