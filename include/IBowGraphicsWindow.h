#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#ifdef _WIN32
	#include <Windows.h>
#endif

namespace Bow {
	namespace Renderer{

		class IGraphicsWindow
		{
		public:
			virtual ~IGraphicsWindow(){}
			virtual void VRelease(void) = 0;

			virtual RenderContextPtr	VGetContext() const = 0;
			virtual HWND				VGetWindowHandle() const = 0;

			virtual int VGetWidth() const = 0;
			virtual int VGetHeight() const = 0;
		};

	}
}