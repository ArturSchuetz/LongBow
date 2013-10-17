#include "OGLColorAttachments.h"

#include <GL\glew.h>

namespace Bow {

	OGLColorAttachments::OGLColorAttachments()
	{
		FN("OGLColorAttachments::OGLColorAttachments()");
    }

	OGLColorAttachments::~OGLColorAttachments()
    {
		FN("OGLColorAttachments::~OGLColorAttachments()");
    }

	Texture2DPtr OGLColorAttachments::GetAttachment(unsigned int index) const
	{
		return std::dynamic_pointer_cast<ITexture2D>(Attachments.at(index).Texture);
	}

	void OGLColorAttachments::SetAttachment(unsigned int index, Texture2DPtr texture)
	{
		LOG_ASSERT(!((texture != nullptr) && (!texture->GetDescription().ColorRenderable())), "Texture must be color renderable but the Description.ColorRenderable property is false.");

		if(Attachments.find(index) == Attachments.end())
			Attachments.insert(std::pair<int, OGLColorAttachment>(index, OGLColorAttachment()));

		if (Attachments.at(index).Texture != texture)
        {
            if ((Attachments.at(index).Texture != nullptr) && (texture == nullptr))
            {
                --m_count;
            }
            else if ((Attachments.at(index).Texture == nullptr) && (texture != nullptr))
            {
                ++m_count;
            }

            Attachments.at(index).Texture = std::dynamic_pointer_cast<OGLTexture2D>(texture);
			Attachments.at(index).Dirty = true;
            IsDirty = true;
        }
	}

	int OGLColorAttachments::GetCount() const
    {
		FN("OGLColorAttachments::GetCount()");
		return m_count;
    }
}