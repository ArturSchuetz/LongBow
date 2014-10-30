#include "BowOGLGraphicsWindow.h"
#include "BowLogger.h"

#include "BowOGLRenderDevice.h"
#include "BowOGLRenderContext.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <unordered_map>

namespace Bow {
	namespace Renderer {
		std::unordered_map<GLFWwindow*, OGLGraphicsWindow*> g_Windows;

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

			if (g_Windows.find(m_Window) == g_Windows.end())
			{
				g_Windows.insert(std::pair<GLFWwindow*, OGLGraphicsWindow*>(m_Window, this));
			}

			if (!m_Window)
			{
				glfwTerminate();
				LOG_ERROR("Error while creating OpenGL-Window with glfw!");
				return false;
			}

			glfwGetWindowSize(m_Window, &m_Width, &m_Height);

			glfwSetWindowSizeCallback(m_Window, OGLGraphicsWindow::ResizeCallback);

			m_Context = OGLRenderContextPtr(new OGLRenderContext(m_Window));

			LOG_DEBUG("OpenGL-Window sucessfully initialized!");
			return m_Context->Initialize();
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
			if (g_Windows.find(m_Window) != g_Windows.end())
			{
				g_Windows.erase(m_Window);
			}
			m_Window = nullptr;

			LOG_DEBUG("OGLGraphicsWindow released");
		}


		RenderContextPtr OGLGraphicsWindow::VGetContext() const
		{
			return m_Context;
		}


		int OGLGraphicsWindow::VGetWidth() const
		{
			return m_Width;
		}


		int OGLGraphicsWindow::VGetHeight() const
		{
			return m_Height;
		}


		bool OGLGraphicsWindow::VShouldClose() const
		{
			return glfwWindowShouldClose(m_Window) != 0;
		}

		void OGLGraphicsWindow::ResizeCallback(GLFWwindow* window, int width, int height)
		{
			if (g_Windows.find(window) != g_Windows.end())
			{
				g_Windows[window]->m_Width = width;
				g_Windows[window]->m_Height = height;
			}
		}
	}
}