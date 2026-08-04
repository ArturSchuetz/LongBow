#include <OpenGL3xRenderDevice/Device/Context/VertexAttributeBindings/BowOGL3xVertexBufferAttributes.h>

#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xVertexBuffer.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

#include <algorithm> // std::max

namespace bow
{

OGLVertexBufferAttributes::OGLVertexBufferAttributes() : m_count(0), m_maximumArrayIndex(0), m_dirty(false) { FN("OGLVertexBufferAttributes::OGLVertexBufferAttributes"); }

VertexBufferAttributeMap OGLVertexBufferAttributes::GetAttributes()
{
    FN("OGLVertexBufferAttributes::GetAttributes");

    VertexBufferAttributeMap reslut;
    for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
    {
        reslut.insert(std::pair<uint32_t, VertexBufferAttributePtr>(it->first, it->second.VertexBufferAttribute));
    }
    return reslut;
}

VertexBufferAttributePtr OGLVertexBufferAttributes::GetAttribute(int index)
{
    FN("OGLVertexBufferAttributes::GetAttribute");

    return m_Attributes[index].VertexBufferAttribute;
}

void OGLVertexBufferAttributes::SetAttribute(int location, VertexBufferAttributePtr value)
{
    FN("OGLVertexBufferAttributes::SetAttribute");

    if (m_Attributes[location].VertexBufferAttribute != value)
    {
        if (value != nullptr)
        {
            LOG_ASSERT(!(value->GetNumberOfComponents() < 1 || value->GetNumberOfComponents() > 4), "NumberOfComponents must be between one and four.");

            if (value->Normalize())
            {
                LOG_ASSERT(!((value->GetComponentDatatype() != ComponentDatatype::Byte) && (value->GetComponentDatatype() != ComponentDatatype::UnsignedByte) && (value->GetComponentDatatype() != ComponentDatatype::Short) &&
                             (value->GetComponentDatatype() != ComponentDatatype::UnsignedShort) && (value->GetComponentDatatype() != ComponentDatatype::Int) && (value->GetComponentDatatype() != ComponentDatatype::UnsignedInt)),
                           "When Normalize is true, ComponentDatatype must be "
                           "Byte, UnsignedByte, Short, "
                           "UnsignedShort, Int, or UnsignedInt.");
            }
        }

        if ((m_Attributes[location].VertexBufferAttribute != nullptr) && (value == nullptr))
        {
            --m_count;
        }
        else if ((m_Attributes[location].VertexBufferAttribute == nullptr) && (value != nullptr))
        {
            ++m_count;
        }

        m_Attributes[location].VertexBufferAttribute = value;
        m_Attributes[location].Dirty = true;
        m_dirty = true;
    }
}

void OGLVertexBufferAttributes::Clean(uint32_t vertexArray)
{
    FN("OGLVertexBufferAttributes::Clean");

    if (m_dirty)
    {
        int maximumArrayIndex = 0;

        for (auto it = m_Attributes.begin(); it != m_Attributes.end(); it++)
        {
            VertexBufferAttributePtr attribute = m_Attributes[it->first].VertexBufferAttribute;

            if (m_Attributes[it->first].Dirty)
            {
                if (attribute != nullptr)
                {
                    Attach(vertexArray, it->first);
                }
                else
                {
                    Detach(vertexArray, it->first);
                }

                m_Attributes[it->first].Dirty = false;
            }

            if (attribute != nullptr)
            {
                maximumArrayIndex = (((NumberOfVertices(attribute) - 1) > maximumArrayIndex) ? (NumberOfVertices(attribute) - 1) : maximumArrayIndex);
            }
        }

        m_dirty = false;
        m_maximumArrayIndex = maximumArrayIndex;
    }
}

int OGLVertexBufferAttributes::GetMaximumArrayIndex()
{
    FN("OGLVertexBufferAttributes::GetMaximumArrayIndex");

    LOG_ASSERT(!m_dirty, "MaximumArrayIndex is not valid until Clean() is called.");
    return m_maximumArrayIndex;
}

void OGLVertexBufferAttributes::Attach(uint32_t vertexArray, int location)
{
    FN("OGLVertexBufferAttributes::Attach");

    VertexBufferAttributePtr attribute = m_Attributes[location].VertexBufferAttribute;
    OGLVertexBufferPtr bufferObjectGL = std::dynamic_pointer_cast<OGLVertexBuffer>(attribute->GetVertexBuffer());

    LOG_TRACE("glEnableVertexArrayAttrib");
    glEnableVertexArrayAttrib(vertexArray, location);

    // glVertexAttribPointer folded three things into one call: which buffer to
    // read from, where in it to start, and how a vertex is laid out. The
    // separate-attribute-format path splits them, which is why the offset moves
    // to the buffer binding and the format is left with a relative offset of
    // zero -- every attribute here has a buffer of its own. DirectX 11 input
    // layouts draw the same line, so stage 3 inherits the shape.
    const uint32_t bindingIndex = (uint32_t)location;

    LOG_TRACE("glVertexArrayVertexBuffer");
    glVertexArrayVertexBuffer(vertexArray, bindingIndex, bufferObjectGL->GetGLHandle(), (GLintptr)attribute->GetOffsetInBytes(), (GLsizei)attribute->GetStrideInBytes());

    LOG_TRACE("glVertexArrayAttribFormat");
    glVertexArrayAttribFormat(vertexArray, location, attribute->GetNumberOfComponents(), OGLTypeConverter::To(attribute->GetComponentDatatype()), attribute->Normalize(), 0);

    LOG_TRACE("glVertexArrayAttribBinding");
    glVertexArrayAttribBinding(vertexArray, location, bindingIndex);
}

void OGLVertexBufferAttributes::Detach(uint32_t vertexArray, int location)
{
    FN("OGLVertexBufferAttributes::Detach");

    LOG_TRACE("glDisableVertexArrayAttrib");
    glDisableVertexArrayAttrib(vertexArray, location);
}

int OGLVertexBufferAttributes::NumberOfVertices(VertexBufferAttributePtr attribute)
{
    FN("OGLVertexBufferAttributes::NumberOfVertices");

    return attribute->GetVertexBuffer()->VGetSizeInBytes() / attribute->GetStrideInBytes();
}

} // namespace bow
