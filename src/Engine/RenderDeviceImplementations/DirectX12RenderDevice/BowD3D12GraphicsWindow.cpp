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

	D3DGraphicsWindow::D3DGraphicsWindow()
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
		case WindowType::Fullscreen:
			fullScreen = true;
			break;
		}

		if (fullScreen)
		{
			HMONITOR hmon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hmon, &mi);

			width = mi.rcMonitor.right - mi.rcMonitor.left;
			height = mi.rcMonitor.bottom - mi.rcMonitor.top;
		}

		HINSTANCE hInstance = GetModuleHandle(NULL);

		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"DirectX";
		windowClass.lpszMenuName = string2widestring(title).c_str();
		RegisterClassEx(&windowClass);

		RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and store a handle to it.
		m_hwnd = CreateWindowW(
			windowClass.lpszClassName,
			string2widestring(title).c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,		// We have no parent window.
			nullptr,		// We aren't using menus.
			hInstance,
			this);

		RECT clientRect = {};
		::GetClientRect(m_hwnd, &clientRect);

		m_width = clientRect.right - clientRect.left;
		m_height = clientRect.bottom - clientRect.top;

		m_ParentDevice = parent;
		m_Context = D3DRenderContextPtr(new D3DRenderContext(m_hwnd, m_ParentDevice));

		bool result = m_Context->Initialize(m_width, m_height);

		if (result)
			ShowWindow(m_hwnd, 10);

		LOG_TRACE("DirectX12-Window sucessfully initialized!");
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

	// Main message handler for the sample.
	LRESULT CALLBACK D3DGraphicsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		D3DGraphicsWindow* window = reinterpret_cast<D3DGraphicsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
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

				window->m_width = clientRect.right - clientRect.left;
				window->m_height = clientRect.bottom - clientRect.top;
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
