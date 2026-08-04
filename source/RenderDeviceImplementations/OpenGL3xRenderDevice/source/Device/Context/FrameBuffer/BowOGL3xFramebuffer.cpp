#include <CoreSystems/BowLogger.h>
#include <OpenGL3xRenderDevice/Device/Context/FrameBuffer/BowOGL3xFramebuffer.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTexture2D.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLFramebuffer::OGLFramebuffer() : m_FramebufferHandle(0), m_ColorAttachments(), m_DepthAttachment(nullptr), m_DepthStencilAttachment(nullptr), m_DirtyFlags(DirtyFlags::None)
{
    FN("OGLFramebuffer::OGLFramebuffer");

    m_FramebufferHandle = 0;
    LOG_TRACE("glGenFramebuffers");
    glGenFramebuffers(1, &m_FramebufferHandle);

    m_DepthAttachment = nullptr;
    m_DepthStencilAttachment = nullptr;
}

OGLFramebuffer::~OGLFramebuffer()
{
    FN("OGLFramebuffer::~OGLFramebuffer");

    if (m_FramebufferHandle != 0)
    {
        LOG_TRACE("glDeleteFramebuffers");
        glDeleteFramebuffers(1, &m_FramebufferHandle);
        m_FramebufferHandle = 0;
    }
}

void OGLFramebuffer::Bind()
{
    FN("OGLFramebuffer::Bind");

    LOG_TRACE("glBindFramebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle);
}

void OGLFramebuffer::UnBind()
{
    FN("OGLFramebuffer::UnBind");

    LOG_TRACE("glBindFramebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OGLFramebuffer::Clean()
{
    FN("OGLFramebuffer::Clean");

    if (m_ColorAttachments.IsDirty)
    {
        OGLColorAttachmentMap colorAttachments = m_ColorAttachments.Attachments;

        GLenum *drawBuffers = new GLenum[colorAttachments.size()];
        for (auto it = colorAttachments.begin(); it != colorAttachments.end(); it++)
        {
            if ((*it).second.Dirty)
            {
                Attach(GL_COLOR_ATTACHMENT0 + (*it).first, (*it).second.Texture);
                (*it).second.Dirty = false;
            }

            if ((*it).second.Texture != nullptr)
            {
                drawBuffers[(*it).first] = GL_COLOR_ATTACHMENT0 + (*it).first;
            }
        }
        LOG_TRACE("glDrawBuffers");
        glDrawBuffers(colorAttachments.size(), drawBuffers);
        delete[] drawBuffers;

        m_ColorAttachments.IsDirty = false;
    }

    if (m_DirtyFlags != DirtyFlags::None)
    {
        if ((DirtyFlags)((int)m_DirtyFlags & (int)DirtyFlags::DepthAttachment) == DirtyFlags::DepthAttachment)
        {
            Attach(GL_DEPTH_ATTACHMENT, m_DepthAttachment);
        }

        //
        // The depth-stencil attachment overrides the depth attachment:
        //
        //    "Attaching a level of a texture to GL_DEPTH_STENCIL_ATTACHMENT
        //     is equivalent to attaching that level to both the
        //     GL_DEPTH_ATTACHMENT and the GL_STENCIL_ATTACHMENT attachment
        //     points simultaneously."
        //
        // We do not expose just a stencil attachment because TextureFormat
        // does not contain a stencil only format.

        if ((DirtyFlags)((int)m_DirtyFlags & (int)DirtyFlags::DepthStencilAttachment) == DirtyFlags::DepthStencilAttachment)
        {
            Attach(GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttachment);
        }

        m_DirtyFlags = DirtyFlags::None;
    }
}

Texture2DPtr OGLFramebuffer::VGetColorAttachment(uint32_t index) const
{
    FN("OGLFramebuffer::VGetColorAttachment");

    return m_ColorAttachments.VGetAttachment(index);
}

void OGLFramebuffer::VSetColorAttachment(uint32_t index, Texture2DPtr texture)
{
    FN("OGLFramebuffer::VSetColorAttachment");

    m_ColorAttachments.VSetAttachment(index, texture);
}

Texture2DPtr OGLFramebuffer::VGetDepthAttachment() const
{
    FN("OGLFramebuffer::VGetDepthAttachment");

    return m_DepthAttachment;
}

void OGLFramebuffer::VSetDepthAttachment(Texture2DPtr texture)
{
    FN("OGLFramebuffer::VSetDepthAttachment");

    if (m_DepthAttachment != texture)
    {
        LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().DepthRenderable())), "Texture must be depth renderable but the "
                                                                                               "Description.DepthRenderable property is false.");

        m_DepthAttachment = std::dynamic_pointer_cast<OGLTexture2D>(texture);
        m_DirtyFlags = (DirtyFlags)((int)m_DirtyFlags | (int)DirtyFlags::DepthAttachment);
    }
}

Texture2DPtr OGLFramebuffer::VGetDepthStencilAttachment() const
{
    FN("OGLFramebuffer::VGetDepthStencilAttachment");

    return m_DepthStencilAttachment;
}

void OGLFramebuffer::VSetDepthStencilAttachment(Texture2DPtr texture)
{
    FN("OGLFramebuffer::VSetDepthStencilAttachment");

    if (m_DepthStencilAttachment != texture)
    {
        LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().DepthStencilRenderable())), "Texture must be depth/stencil renderable but the "
                                                                                                      "Description.DepthStencilRenderable property is false.");

        m_DepthStencilAttachment = std::dynamic_pointer_cast<OGLTexture2D>(texture);
        m_DirtyFlags = (DirtyFlags)((int)m_DirtyFlags | (int)DirtyFlags::DepthStencilAttachment);
    }
}

void OGLFramebuffer::Attach(uint32_t attachPoint, OGLTexture2DPtr texture)
{
    FN("OGLFramebuffer::Attach");

    if (texture != nullptr)
    {
        // TODO:  Mipmap level
        // glFramebufferTexture(GL_FRAMEBUFFER, attachPoint,
        // texture->GetHandle(), 0); <== Does not work for OpenGL3.1
        LOG_TRACE("glFramebufferTexture2D");
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachPoint, GL_TEXTURE_2D, texture->GetHandle(), 0);
    }
    else
    {
        LOG_TRACE("glFramebufferTexture2D");
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachPoint, GL_TEXTURE_2D, 0, 0);
    }
}

} // namespace bow
