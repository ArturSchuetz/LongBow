#pragma once
#include "LongBow.h"
#include "IBowColorAttachments.h"

#include "OGLTexture2D.h"

namespace Bow {

	struct OGLColorAttachment
    {
	public:
		OGLTexture2DPtr		Texture;
        bool				Dirty;
    };

	typedef std::hash_map<unsigned int, OGLColorAttachment> OGLColorAttachmentMap;

	class OGLColorAttachments : public IColorAttachments
    {
	public:
		OGLColorAttachments();
		~OGLColorAttachments();

		Texture2DPtr GetAttachment(unsigned int index) const; 
		void SetAttachment(unsigned int index, Texture2DPtr texture);

		int GetCount() const;

		bool IsDirty;
		OGLColorAttachmentMap Attachments;

	private:
		int m_count;
    };

	typedef std::shared_ptr<OGLColorAttachments> OGLColorAttachmentsPtr;

}