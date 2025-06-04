/**
 * @file IBowFramebuffer.h
 * @brief Declarations for IBowFramebuffer.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	class IFramebuffer
	{
	public:
		virtual ~IFramebuffer(){}

		virtual Texture2DPtr	VGetColorAttachment(int OutputLocation) const = 0;
		virtual void			VSetColorAttachment(int OutputLocation, Texture2DPtr texture) = 0;

		virtual Texture2DPtr	VGetDepthAttachment(void) const = 0;
		virtual void			VSetDepthAttachment(Texture2DPtr texture) = 0;

		virtual Texture2DPtr	VGetDepthStencilAttachment(void) const = 0;
		virtual void			VSetDepthStencilAttachment(Texture2DPtr texture) = 0;
	};

}
