#include <OpenGL3xRenderDevice/Device/Context/VertexAttributeBindings/BowOGL3xVertexAttributeBindings.h>

#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xIndexBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xVertexBuffer.h>

#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

namespace bow
{

OGLVertexAttributeBindings::OGLVertexAttributeBindings() : m_dirtyIndexBuffer(false), m_VertexArrayHandle(0)
{
    FN("OGLVertexAttributeBindings::OGLVertexAttributeBindings");

    LOG_TRACE("glGenVertexAttributeBindingss");
    glGenVertexArrays(1, &m_VertexArrayHandle);
}

OGLVertexAttributeBindings::~OGLVertexAttributeBindings()
{
    FN("OGLVertexAttributeBindings::~OGLVertexAttributeBindings");

    if (m_VertexArrayHandle != 0)
    {
        LOG_TRACE("glDeleteVertexAttributeBindingss");
        glDeleteVertexArrays(1, &m_VertexArrayHandle);
        m_VertexArrayHandle = 0;
    }
}

void OGLVertexAttributeBindings::Bind()
{
    FN("OGLVertexAttributeBindings::Bind");

    LOG_TRACE("glBindVertexArray");
    glBindVertexArray(m_VertexArrayHandle);
}

void OGLVertexAttributeBindings::Clean()
{
    FN("OGLVertexAttributeBindings::Clean");

    m_Attributes.Clean();

    if (m_dirtyIndexBuffer)
    {
        if (m_indexBuffer.get() != nullptr)
        {
            m_indexBuffer->Bind();
        }
        else
        {
            OGLIndexBuffer::UnBind();
        }

        m_dirtyIndexBuffer = false;
    }
}

uint64_t OGLVertexAttributeBindings::MaximumArrayIndex()
{
    FN("OGLVertexAttributeBindings::MaximumArrayIndex");

    return m_Attributes.GetMaximumArrayIndex();
}

VertexBufferAttributeMap OGLVertexAttributeBindings::VGetAttributes()
{
    FN("OGLVertexAttributeBindings::VGetAttributes");

    return m_Attributes.GetAttributes();
}

void OGLVertexAttributeBindings::VSetAttribute(uint32_t location, VertexBufferAttributePtr pointer)
{
    FN("OGLVertexAttributeBindings::VSetAttribute");

    LOG_ASSERT(pointer != nullptr, "Vertex Buffer Attribute not found!");
    m_Attributes.SetAttribute(location, pointer);
}

void OGLVertexAttributeBindings::VSetAttribute(ShaderVertexAttributePtr vertexAttribute, VertexBufferAttributePtr pointer)
{
    FN("OGLVertexAttributeBindings::VSetAttribute");

    LOG_ASSERT(vertexAttribute != nullptr, "Shader Vertex Attribute not found!");
    m_Attributes.SetAttribute(vertexAttribute->Location, pointer);
}

IndexBufferPtr OGLVertexAttributeBindings::VGetIndexBuffer()
{
    FN("OGLVertexAttributeBindings::VGetIndexBuffer");

    return std::dynamic_pointer_cast<IIndexBuffer>(m_indexBuffer);
}

void OGLVertexAttributeBindings::VSetIndexBuffer(IndexBufferPtr indexBufferPtr)
{
    FN("OGLVertexAttributeBindings::VSetIndexBuffer");

    m_indexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(indexBufferPtr);
    m_dirtyIndexBuffer = true;
}

} // namespace bow
