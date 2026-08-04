#pragma once
#include <OpenGL3xRenderDevice/Device/Context/FrameBuffer/BowOGL3xColorAttachments.h>
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/FrameBuffer/IBowFramebuffer.h>

namespace bow
{

typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;
typedef std::shared_ptr<class OGLFramebufferName> OGLFramebufferNamePtr;

class OGLFramebuffer : public IFramebuffer
{
  public:
    OGLFramebuffer();
    ~OGLFramebuffer();

    void Bind();
    static void UnBind();
    void Clean();

    Texture2DPtr VGetColorAttachment(uint32_t index) const;
    void VSetColorAttachment(uint32_t index, Texture2DPtr texture);

    Texture2DPtr VGetDepthAttachment() const;
    void VSetDepthAttachment(Texture2DPtr texture);

    Texture2DPtr VGetDepthStencilAttachment() const;
    void VSetDepthStencilAttachment(Texture2DPtr texture);

    //! Attaches a texture to this framebuffer.
    /*!
        Not static any more: with direct state access the call names the
        framebuffer instead of acting on whichever one happens to be bound.
    */
    void Attach(uint32_t attachPoint, OGLTexture2DPtr texture);

  private:
    // You shall not copy
    OGLFramebuffer(const OGLFramebuffer &obj) = delete;
    OGLFramebuffer &operator=(const OGLFramebuffer &obj) = delete;

    enum class DirtyFlags : char
    {
        None = 0,
        DepthAttachment = 1,
        DepthStencilAttachment = 2
    };

    OGLColorAttachments m_ColorAttachments;
    OGLTexture2DPtr m_DepthAttachment;
    OGLTexture2DPtr m_DepthStencilAttachment;
    DirtyFlags m_DirtyFlags;

    uint32_t m_FramebufferHandle; // Name
};

typedef std::shared_ptr<OGLFramebuffer> OGLFramebufferPtr;

} // namespace bow
