#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	class IColorAttachments
	{
	public:
		virtual ~IColorAttachments(){}

		virtual Texture2DPtr	VGetAttachment(unsigned int index) const = 0;
		virtual void			VSetAttachment(unsigned int index, Texture2DPtr texture) = 0;
	};

}
