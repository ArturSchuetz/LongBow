#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IFramebuffer
		{
		public:
			virtual ~IFramebuffer(){}

			virtual Texture2DPtr	GetColorAttachment(int OutputLocation) const = 0;
			virtual void			SetColorAttachment(int OutputLocation, Texture2DPtr texture) = 0;

			virtual Texture2DPtr	GetDepthAttachment(void) const = 0;
			virtual void			SetDepthAttachment(Texture2DPtr texture) = 0;

			virtual Texture2DPtr	GetDepthStencilAttachment(void) const = 0;
			virtual void			SetDepthStencilAttachment(Texture2DPtr texture) = 0;
		};

	}
}