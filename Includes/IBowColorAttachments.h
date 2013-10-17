#pragma once
#include "LongBow.h"
#include "IBowTexture2D.h"

namespace Bow {

	class IColorAttachments
    {
	public:
		virtual Texture2DPtr GetAttachment(unsigned int index) const = 0; 
		virtual void SetAttachment(unsigned int index, Texture2DPtr texture) = 0;
    };

	typedef std::shared_ptr<IColorAttachments> ColorAttachmentsPtr;

}