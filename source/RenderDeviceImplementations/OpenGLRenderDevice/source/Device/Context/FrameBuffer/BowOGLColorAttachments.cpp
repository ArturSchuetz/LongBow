#include <OpenGLRenderDevice/Device/Context/FrameBuffer/BowOGLColorAttachments.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTexture2D.h>

#include <CoreSystems/BowLogger.h>


#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLColorAttachments::OGLColorAttachments() : m_count(0), IsDirty(false), Attachments() { FN("OGLColorAttachments::OGLColorAttachments"); }

OGLColorAttachments::~OGLColorAttachments() { FN("OGLColorAttachments::~OGLColorAttachments"); }

Texture2DPtr OGLColorAttachments::VGetAttachment(uint32_t index) const
{
    FN("OGLColorAttachments::VGetAttachment");

    return std::dynamic_pointer_cast<ITexture2D>(Attachments.at(index).Texture);
}

void OGLColorAttachments::VSetAttachment(uint32_t index, Texture2DPtr texture)
{
    FN("OGLColorAttachments::VSetAttachment");

    LOG_ASSERT(!((texture != nullptr) && (!texture->VGetDescription().ColorRenderable())), "Texture must be color renderable but the Description.ColorRenderable property is false.");

    if (Attachments.find(index) == Attachments.end())
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
    FN("OGLColorAttachments::GetCount");

    return m_count;
}

} // namespace bow
