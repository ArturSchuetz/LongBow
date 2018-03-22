#pragma once
#include "BowPrerequisites.h"

#include "IBowGraphicsWindow.h"

struct GLFWwindow;

namespace bow {

	typedef std::shared_ptr<class OGLRenderContext> OGLRenderContextPtr;
	class OGLRenderDevice;

	class OGLGraphicsWindow : public IGraphicsWindow
	{
	public:
		OGLGraphicsWindow();
		~OGLGraphicsWindow(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		bool Initialize(unsigned int width, unsigned int height, const std::string& title, WindowType windowType, OGLRenderDevice *device);
		void VRelease(void);

		RenderContextPtr VGetContext() const;
		void VPollWindowEvents() const;

#ifdef _WIN32
		HWND VGetHandle() const;
#endif
		void VHideCursor() const;
		void VShowCursor() const;

		int VGetWidth() const;
		int VGetHeight() const;

		bool VIsFocused() const;
		bool VShouldClose() const;

	private:
		static void ResizeCallback(GLFWwindow* window, int width, int height);

		//you shall not copy
		OGLGraphicsWindow(OGLGraphicsWindow&) : m_Context(nullptr), m_ParentDevice(nullptr){}
		OGLGraphicsWindow& operator=(const OGLGraphicsWindow&) { return *this; }

		OGLRenderDevice*	m_ParentDevice;
		OGLRenderContextPtr m_Context;
		GLFWwindow			*m_Window;

		int m_Width;
		int m_Height;
	};

	typedef std::shared_ptr<OGLGraphicsWindow> OGLGraphicsWindowPtr;
	typedef std::unordered_map<unsigned int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;
}
