#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowGraphicsWindow.h"

namespace bow {

	class D3DGraphicsWindow : public IGraphicsWindow
	{
		friend class D3DRenderContext;
	public:
		D3DGraphicsWindow();
		~D3DGraphicsWindow(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================

		bool Initialize(unsigned int width, unsigned int height, const std::string& title, WindowType windowType, D3DRenderDevice* parent);
		void VRelease(void);

		RenderContextPtr VGetContext() const;
		void VPollWindowEvents() const;

		HWND VGetHandle() const;
		void VHideCursor() const;
		void VShowCursor() const;

		int VGetWidth() const;
		int VGetHeight() const;

		bool VIsFocused() const;
		bool VShouldClose() const;

	private:
		//you shall not copy
		D3DGraphicsWindow(D3DGraphicsWindow&) {}
		D3DGraphicsWindow& operator=(const D3DGraphicsWindow&) { return *this; }

		void Resize(unsigned int width, unsigned int height);
		void SetFullscreen(bool fullscreen);

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		D3DRenderDevice*	m_ParentDevice;
		D3DRenderContextPtr m_Context;

		unsigned int m_width;
		unsigned int m_height;

		HWND m_hwnd; 
		RECT m_windowRect;
		bool m_shouldClose;
		bool m_fullScreen;
	};
}
