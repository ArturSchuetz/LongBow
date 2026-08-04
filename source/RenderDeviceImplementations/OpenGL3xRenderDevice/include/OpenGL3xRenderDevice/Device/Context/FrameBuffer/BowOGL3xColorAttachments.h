#pragma once
#include <OpenGL3xRenderDevice/OpenGL3xRenderDevice_api.h>
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/FrameBuffer/IBowColorAttachments.h>

namespace bow
{

typedef std::shared_ptr<class OGLTexture2D> OGLTexture2DPtr;

struct OGLColorAttachment
{
  public:
    OGLTexture2DPtr Texture;
    bool Dirty;
};

typedef std::unordered_map<uint32_t, OGLColorAttachment> OGLColorAttachmentMap;

class OGLColorAttachments : public IColorAttachments
{
  public:
    OGLColorAttachments();
    ~OGLColorAttachments();

    Texture2DPtr VGetAttachment(uint32_t index) const;
    void VSetAttachment(uint32_t index, Texture2DPtr texture);

    int GetCount() const;

    bool IsDirty;
    OGLColorAttachmentMap Attachments;

  private:
    // You shall not copy
    OGLColorAttachments(const OGLColorAttachments &obj) = delete;
    OGLColorAttachments &operator=(const OGLColorAttachments &obj) = delete;

    int m_count;
};

typedef std::shared_ptr<OGLColorAttachments> OGLColorAttachmentsPtr;

} // namespace bow
