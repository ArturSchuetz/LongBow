#include "BowOGLGraphicsWindow.h"
#include "BowOGLRenderContext.h"

namespace Bow {
	namespace Renderer {

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		OGLGraphicsWindow::OGLGraphicsWindow()
		{
			m_Context = nullptr;
			m_hWnd = NULL;
		}


		OGLGraphicsWindow::~OGLGraphicsWindow(void)
		{
			VRelease();
		}


		bool OGLGraphicsWindow::VInitialize(HINSTANCE hInstance, int width, int height, std::string title)
		{
			TCHAR szWindowClass[] = TEXT("OGLWindow");

			WNDCLASSEX wcex;

			wcex.cbSize = sizeof(WNDCLASSEX);

			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInstance;
			//wcex.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HELLOWORLD));
			wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = szWindowClass;
			wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

			RegisterClassEx(&wcex);

			m_hWnd = CreateWindow(szWindowClass, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, NULL, NULL, hInstance, NULL);

			if (!m_hWnd)
			{
				return FALSE;
			}

			RECT rect;
			if (GetClientRect(m_hWnd, &rect))
			{
				m_width = rect.right - rect.left;
				m_height = rect.bottom - rect.top;
			}
			else
			{
				m_width = width;
				m_height = height;
			}

			ShowWindow(m_hWnd, SW_SHOWDEFAULT);
			UpdateWindow(m_hWnd);

			m_Context = OGLRenderContextPtr(new OGLRenderContext());
			return m_Context->VInitialize(m_hWnd, m_width, m_height);
		}


		void OGLGraphicsWindow::VRelease(void)
		{
			if (m_Context.get() != nullptr)
			{
				m_Context->VRelease();
				m_Context.reset();
			}
		}


		RenderContextPtr OGLGraphicsWindow::VGetContext() const
		{
			return m_Context;
		}


		HWND OGLGraphicsWindow::VGetWindowHandle() const
		{
			return m_hWnd;
		}


		int OGLGraphicsWindow::VGetWidth() const
		{
			return m_width;
		}


		int OGLGraphicsWindow::VGetHeight() const
		{
			return m_height;
		}


		//
		//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
		//
		//  PURPOSE:  Processes messages for the main window.
		//
		//  WM_DESTROY	- post a quit message and return
		//
		//
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			case WM_SETFOCUS:
				break;

			case WM_KILLFOCUS:
				break;

			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return 0;
		}

	}
}