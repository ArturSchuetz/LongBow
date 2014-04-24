#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class IColorAttachments
		{
		public:
			virtual ~IColorAttachments(){}

			virtual Texture2DPtr	GetAttachment(unsigned int index) const = 0;
			virtual void			SetAttachment(unsigned int index, Texture2DPtr texture) = 0;
		};

	}
}