#pragma once
#include "BowPrerequisites.h"

#include "IBowGraphicsWindow.h"

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
			bool VInitialize(HINSTANCE hInstance, int width, int height, std::string title);
			void VRelease(void);

			RenderContextPtr VGetContext() const;
			HWND VGetWindowHandle() const;

			int VGetWidth() const;
			int VGetHeight() const;

		private:
			OGLRenderContextPtr m_Context;
			HWND m_hWnd;

			int m_width;
			int m_height;
		};

		typedef std::shared_ptr<OGLGraphicsWindow> OGLGraphicsWindowPtr;
		typedef std::hash_map<int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;
	}
}