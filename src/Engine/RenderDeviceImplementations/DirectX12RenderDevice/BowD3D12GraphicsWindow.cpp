#include "BowD3D12GraphicsWindow.h"
#include "BowLogger.h"

#include "BowD3D12RenderDevice.h"
#include "BowD3D12RenderContext.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"


namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);

	D3DGraphicsWindow::D3DGraphicsWindow() :
		m_fullScreen(false)
	{
		m_shouldClose = false;
	}

	bool D3DGraphicsWindow::Initialize(unsigned int width, unsigned int height, const std::string& title, WindowType windowType, D3DRenderDevice *parent)
	{
		// is window full screen?
		bool fullScreen = false;
		switch (windowType)
		{
		case WindowType::Windowed:
			fullScreen = false;
			break;
		case WindowType::FullscreenBorderlessWindow:
			fullScreen = true;
			break;
		case WindowType::Fullscreen:
			fullScreen = true;
			break;
		}
		
		HINSTANCE hInstance = GetModuleHandle(NULL);

		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"LongbowDirectX12";
		windowClass.lpszMenuName = string2widestring(title).c_str();
		RegisterClassEx(&windowClass);

		int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
		int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
		int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

		// Create the window and store a handle to it.
		m_hwnd = CreateWindowExW(
			NULL,
			windowClass.lpszClassName,
			string2widestring(title).c_str(),
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			nullptr,		// We have no parent window.
			nullptr,		// We aren't using menus.
			hInstance,
			this);

		// Initialize the global window rect variable.
		::GetWindowRect(m_hwnd, &m_windowRect);

		m_ParentDevice = parent;
		m_Context = D3DRenderContextPtr(new D3DRenderContext(m_hwnd, m_ParentDevice));

		bool result = m_Context->Initialize(m_width, m_height);

		if (result)
		{
			if(fullScreen)
				SetFullscreen(true);
			else
				::ShowWindow(m_hwnd, SW_SHOW);
		}

		LOG_INFO("DirectX12-Window sucessfully initialized!");
		return result;
	}

	D3DGraphicsWindow::~D3DGraphicsWindow(void)
	{
		VRelease();
	}

	void D3DGraphicsWindow::VRelease(void)
	{

	}

	HWND D3DGraphicsWindow::VGetHandle() const
	{
		return m_hwnd;
	}

	RenderContextPtr D3DGraphicsWindow::VGetContext() const
	{
		return m_Context;
	}

	void D3DGraphicsWindow::VPollWindowEvents() const
	{
		// Main sample loop.
		MSG msg = {};
		if (msg.message != WM_QUIT)
		{
			// Process any messages in the queue.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	void D3DGraphicsWindow::VHideCursor() const
	{
		// TODO
	}

	void D3DGraphicsWindow::VShowCursor() const
	{
		// TODO
	}

	int D3DGraphicsWindow::VGetWidth() const
	{
		return m_width;
	}

	int D3DGraphicsWindow::VGetHeight() const
	{
		return m_height;
	}

	bool D3DGraphicsWindow::VIsFocused() const
	{
		// TODO
		return false;
	}

	bool D3DGraphicsWindow::VShouldClose() const
	{
		VPollWindowEvents();
		return m_shouldClose;
	}

	void D3DGraphicsWindow::Resize(unsigned int width, unsigned int height)
	{
		if (m_width != width || m_height != height)
		{
			// Don't allow 0 size swap chain back buffers.
			m_width = std::max(1u, width);
			m_height = std::max(1u, height);

			m_Context->Resize(m_width, m_height);
		}
	}

	void D3DGraphicsWindow::SetFullscreen(bool fullscreen)
	{
		if (m_fullScreen != fullscreen)
		{
			m_fullScreen = fullscreen;

			if (m_fullScreen) // Switching to fullscreen.
			{
				// Store the current window dimensions so they can be restored 
				// when switching out of fullscreen state.
				::GetWindowRect(m_hwnd, &m_windowRect);

				// Set the window style to a borderless window so the client area fills
				// the entire screen.
				UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

				::SetWindowLongW(m_hwnd, GWL_STYLE, windowStyle);

				// Query the name of the nearest display device for the window.
				// This is required to set the fullscreen dimensions of the window
				// when using a multi-monitor setup.
				HMONITOR hMonitor = ::MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFOEX monitorInfo = {};
				monitorInfo.cbSize = sizeof(MONITORINFOEX);
				::GetMonitorInfo(hMonitor, &monitorInfo);

				::SetWindowPos(m_hwnd, HWND_TOPMOST,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				::ShowWindow(m_hwnd, SW_MAXIMIZE);
			}
			else
			{
				// Restore all the window decorators.
				::SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

				::SetWindowPos(m_hwnd, HWND_NOTOPMOST,
					m_windowRect.left,
					m_windowRect.top,
					m_windowRect.right - m_windowRect.left,
					m_windowRect.bottom - m_windowRect.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				::ShowWindow(m_hwnd, SW_NORMAL);
			}

			RECT clientRect = {};
			::GetClientRect(m_hwnd, &clientRect);

			m_width = clientRect.right - clientRect.left;
			m_height = clientRect.bottom - clientRect.top;
		}
	}

	// Main message handler for the sample.
	LRESULT CALLBACK D3DGraphicsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		D3DGraphicsWindow* window = reinterpret_cast<D3DGraphicsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

				switch (wParam)
				{
				case VK_RETURN:
				case VK_F11:
					if (alt) {
						window->SetFullscreen(!window->m_fullScreen);
					}
					break;
				}
			}
			break;
			case WM_CREATE:
			{
				// Save the DXSample* passed in to CreateWindow.
				LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
				return 0;
			}
			break;
			case WM_SIZE:
			{
				RECT clientRect = {};
				::GetClientRect(hWnd, &clientRect);

				window->Resize(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
				return 0;
			}
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				window->m_shouldClose = true;
				return 0;
			}
			default:
				return ::DefWindowProcW(hWnd, message, wParam, lParam);
		}
	}
}
