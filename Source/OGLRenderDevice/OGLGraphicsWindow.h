#pragma once
#include "LongBow.h"
#include "IBowGraphicsWindow.h"
#include "OGLRenderContext.h"

namespace Bow {

	class OGLGraphicsWindow : public IGraphicsWindow
	{
	public:
		OGLGraphicsWindow();
		~OGLGraphicsWindow(void);
		
		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		virtual bool VInitialize(HINSTANCE hInstance, int width, int height, std::string title);
		virtual void VRelease(void);

		RenderContextPtr VGetContext() const;
		HWND VGetWindowHandle() const;

		virtual int VGetWidth() const;
		virtual int VGetHeight() const;

	private:
		OGLRenderContextPtr m_Context;
		HWND m_hWnd;

		int m_width;
		int m_height;
	};
	
	typedef std::shared_ptr<OGLGraphicsWindow> OGLGraphicsWindowPtr;
	typedef std::hash_map<int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;
}