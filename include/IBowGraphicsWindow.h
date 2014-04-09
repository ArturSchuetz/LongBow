#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IGraphicsWindow
		{
		public:
			virtual ~IGraphicsWindow(){}
			virtual void VRelease(void) = 0;

			virtual RenderContextPtr	VGetContext() const = 0;

			virtual int VGetWidth() const = 0;
			virtual int VGetHeight() const = 0;
			virtual bool VShouldClose() const = 0;
		};

	}
}