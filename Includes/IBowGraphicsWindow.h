#pragma once
#include "LongBow.h"
#include "IBowRenderContext.h"

namespace Bow {

	class IGraphicsWindow
	{
	public:
		virtual ~IGraphicsWindow(){}
		virtual void VRelease(void)=0;

		virtual RenderContextPtr VGetContext() const = 0;
		virtual HWND			 VGetWindowHandle() const = 0;

		virtual int VGetWidth() const = 0;
		virtual int VGetHeight() const = 0;
	};

	typedef std::shared_ptr<IGraphicsWindow> GraphicsWindowPtr;
	typedef std::hash_map<int, GraphicsWindowPtr> GraphicsWindowMap;

}