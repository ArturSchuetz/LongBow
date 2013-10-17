#pragma once
#include "LongBow.h"
#include "IBowFramebuffer.h"

#include "OGLFramebuffer.h"
#include "OGLColorAttachments.h"
#include "OGLFramebufferName.h"

namespace Bow {

	class OGLFramebuffer : public IFramebuffer
    {
	public:
		OGLFramebuffer::OGLFramebuffer();
		OGLFramebuffer::~OGLFramebuffer();

        void		Bind();
        static void UnBind();
        void		Clean();

		Texture2DPtr	GetColorAttachment(int index) const;
		void			SetColorAttachment(int index, Texture2DPtr texture);

        Texture2DPtr	GetDepthAttachment(void) const;
		void			SetDepthAttachment(Texture2DPtr texture);

        Texture2DPtr	GetDepthStencilAttachment(void) const;
		void			SetDepthStencilAttachment(Texture2DPtr texture);

		static void Attach(unsigned int attachPoint, OGLTexture2DPtr texture);

	private:
		enum class DirtyFlags : char
        {
            None = 0,
            DepthAttachment = 1,
            DepthStencilAttachment = 2
        };

		OGLFramebufferNamePtr	m_name;
        OGLColorAttachments		m_colorAttachments;
        OGLTexture2DPtr			m_depthAttachment;
        OGLTexture2DPtr			m_depthStencilAttachment;
        DirtyFlags				m_dirtyFlags;
    };

	typedef std::shared_ptr<OGLFramebuffer> OGLFramebufferPtr;

}