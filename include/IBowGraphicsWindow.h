/**
 * @file IBowGraphicsWindow.h
 * @brief Declarations for IBowGraphicsWindow.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bow {

	class IGraphicsWindow
	{
	public:
		virtual ~IGraphicsWindow(){}
		virtual void VRelease(void) = 0;

		virtual RenderContextPtr VGetContext() const = 0;
		virtual void VPollWindowEvents() const = 0;

#ifdef _WIN32
		virtual HWND VGetHandle() const = 0;
#endif

		virtual void VHideCursor() const = 0;
		virtual void VShowCursor() const = 0;

		virtual int VGetWidth() const = 0;
		virtual int VGetHeight() const = 0;

		virtual bool VIsFocused() const = 0;
		virtual bool VShouldClose() const = 0;
	};

}
