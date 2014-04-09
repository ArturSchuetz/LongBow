#pragma once
#include "BowPrerequisites.h"

#include "IBowGraphicsWindow.h"

struct GLFWwindow;

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLRenderContext> OGLRenderContextPtr;

		class OGLGraphicsWindow : public IGraphicsWindow
		{
		public:
			OGLGraphicsWindow();
			~OGLGraphicsWindow(void);

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			bool Initialize(unsigned int width, unsigned int height, const std::string& title, WindowType windowType);
			void VRelease(void);

			RenderContextPtr VGetContext() const;

			int VGetWidth() const;
			int VGetHeight() const;
			bool VShouldClose() const;

		private:
			//you shall not copy
			OGLGraphicsWindow(OGLGraphicsWindow&) : m_Context(nullptr){}
			OGLGraphicsWindow& operator=(const OGLGraphicsWindow&) { return *this; }

			OGLRenderContextPtr m_Context;
			GLFWwindow			*m_Window;
		};

		typedef std::shared_ptr<OGLGraphicsWindow> OGLGraphicsWindowPtr;
		typedef std::hash_map<int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;
	}
}