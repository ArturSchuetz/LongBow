#pragma once
#include "BowPrerequisites.h"

#include "IBowColorAttachments.h"

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;

		struct OGLColorAttachment
		{
		public:
			OGLTexture2DPtr		Texture;
			bool				Dirty;
		};

		typedef std::unordered_map<unsigned int, OGLColorAttachment> OGLColorAttachmentMap;

		class OGLColorAttachments : public IColorAttachments
		{
		public:
			OGLColorAttachments();
			~OGLColorAttachments();

			Texture2DPtr VGetAttachment(unsigned int index) const;
			void VSetAttachment(unsigned int index, Texture2DPtr texture);

			int GetCount() const;

			bool IsDirty;
			OGLColorAttachmentMap Attachments;

		private:
			int m_count;
		};

		typedef std::shared_ptr<OGLColorAttachments> OGLColorAttachmentsPtr;

	}
}