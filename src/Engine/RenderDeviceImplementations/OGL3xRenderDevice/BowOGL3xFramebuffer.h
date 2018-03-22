#pragma once
#include "BowPrerequisites.h"

#include "IBowFramebuffer.h"
#include "BowOGL3xColorAttachments.h"

namespace bow {

	typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
	typedef std::shared_ptr<class OGLFramebufferName> OGLFramebufferNamePtr;

	class OGLFramebuffer : public IFramebuffer
	{
	public:
		OGLFramebuffer::OGLFramebuffer();
		OGLFramebuffer::~OGLFramebuffer();

		void		Bind();
		static void UnBind();
		void		Clean();

		Texture2DPtr	VGetColorAttachment(int index) const;
		void			VSetColorAttachment(int index, Texture2DPtr texture);

		Texture2DPtr	VGetDepthAttachment(void) const;
		void			VSetDepthAttachment(Texture2DPtr texture);

		Texture2DPtr	VGetDepthStencilAttachment(void) const;
		void			VSetDepthStencilAttachment(Texture2DPtr texture);

		static void Attach(unsigned int attachPoint, OGLTexture2DPtr texture);

	private:
		enum class DirtyFlags : char
		{
			None = 0,
			DepthAttachment = 1,
			DepthStencilAttachment = 2
		};

		OGLColorAttachments		m_ColorAttachments;
		OGLTexture2DPtr			m_DepthAttachment;
		OGLTexture2DPtr			m_DepthStencilAttachment;
		DirtyFlags				m_DirtyFlags;

		unsigned int			m_FramebufferHandle; // Name
	};

	typedef std::shared_ptr<OGLFramebuffer> OGLFramebufferPtr;

}
