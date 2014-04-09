#include "BowOGLGraphicsWindow.h"
#include "BowLogger.h"

#include "BowOGLRenderDevice.h"
#include "BowOGLRenderContext.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace Bow {
	namespace Renderer {

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		OGLGraphicsWindow::OGLGraphicsWindow() :
			m_Context(nullptr)
		{
		}


		bool OGLGraphicsWindow::Initialize(unsigned int width, unsigned int height, const std::string& title, WindowType windowType)
		{
			// Create a fullscrenn window?
			GLFWmonitor* monitor = nullptr;
			if (windowType == WindowType::Fullscreen)
				monitor = glfwGetPrimaryMonitor();

			// Creating Window with a cool custom deleter
			m_Window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);

			if (!m_Window)
			{
				glfwTerminate();
				LOG_ERROR("Error while creating OpenGL-Window with glfw!");
				return false;
			}

			m_Context = OGLRenderContextPtr(new OGLRenderContext(m_Window));

			LOG_DEBUG("OpenGL-Window sucessfully initialized!");
			return m_Context->Initialize(width, height);
		}


		OGLGraphicsWindow::~OGLGraphicsWindow(void)
		{
			VRelease();
		}


		void OGLGraphicsWindow::VRelease(void)
		{
			if (m_Context.get() != nullptr)
			{
				m_Context->VRelease();
				m_Context.reset();
			}
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;

			LOG_DEBUG("OGLGraphicsWindow released");
		}


		RenderContextPtr OGLGraphicsWindow::VGetContext() const
		{
			return m_Context;
		}


		int OGLGraphicsWindow::VGetWidth() const
		{
			int width, height;
			glfwGetWindowSize(m_Window, &width, &height);
			return width;
		}


		int OGLGraphicsWindow::VGetHeight() const
		{
			int width, height;
			glfwGetWindowSize(m_Window, &width, &height);
			return height;
		}


		bool OGLGraphicsWindow::VShouldClose() const
		{
			return glfwWindowShouldClose(m_Window) != 0;
		}

	}
}