#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <RenderDevice/Device/Buffer/BowIndexBufferDatatype.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowComponentDatatype.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

class VertexAttributeBindingsSizes
{
  public:
    static int SizeOf(IndexBufferDatatype type)
    {
        switch (type)
        {
        case IndexBufferDatatype::UnsignedInt8:
            return sizeof(uint8_t);
        case IndexBufferDatatype::UnsignedInt16:
            return sizeof(uint16_t);
        case IndexBufferDatatype::UnsignedInt32:
            return sizeof(uint32_t);
        }

        LOG_FATAL("IndexBufferDatatype does not exist.");
        return -1;
    }

    static int SizeOf(ComponentDatatype type)
    {
        switch (type)
        {
        case ComponentDatatype::Byte:
        case ComponentDatatype::UnsignedByte:
            return sizeof(char);
        case ComponentDatatype::Short:
            return sizeof(short);
        case ComponentDatatype::UnsignedShort:
            return sizeof(uint16_t);
        case ComponentDatatype::Int:
            return sizeof(int);
        case ComponentDatatype::UnsignedInt:
            return sizeof(uint32_t);
        case ComponentDatatype::Float:
            return sizeof(float);
        }

        LOG_FATAL("ComponentDatatype does not exist.");
        return -1;
    }

    static int SizeOf(ShaderVertexAttributeType type)
    {
        switch (type)
        {
        case ShaderVertexAttributeType::Float:
            return sizeof(float);
        case ShaderVertexAttributeType::FloatVector2:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::FloatVector3:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::FloatVector4:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::FloatMatrix22:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::FloatMatrix33:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::FloatMatrix44:
            return sizeof(float) * NumberOfComponents(type);
        case ShaderVertexAttributeType::Int:
            return sizeof(int);
        case ShaderVertexAttributeType::IntVector2:
            return sizeof(int) * NumberOfComponents(type);
        case ShaderVertexAttributeType::IntVector3:
            return sizeof(int) * NumberOfComponents(type);
        case ShaderVertexAttributeType::IntVector4:
            return sizeof(int) * NumberOfComponents(type);
        }

        LOG_FATAL("ShaderVertexAttributeType does not exist.");
        return -1;
    }

    static int NumberOfComponents(ShaderVertexAttributeType type)
    {
        switch (type)
        {
        case ShaderVertexAttributeType::Float:
            return 1;
        case ShaderVertexAttributeType::FloatVector2:
            return 2;
        case ShaderVertexAttributeType::FloatVector3:
            return 3;
        case ShaderVertexAttributeType::FloatVector4:
            return 4;
        case ShaderVertexAttributeType::FloatMatrix22:
            return 4;
        case ShaderVertexAttributeType::FloatMatrix33:
            return 9;
        case ShaderVertexAttributeType::FloatMatrix44:
            return 16;
        case ShaderVertexAttributeType::Int:
            return 1;
        case ShaderVertexAttributeType::IntVector2:
            return 2;
        case ShaderVertexAttributeType::IntVector3:
            return 3;
        case ShaderVertexAttributeType::IntVector4:
            return 4;
        }

        LOG_FATAL("ShaderVertexAttributeType does not exist.");
        return -1;
    }
};

} // namespace bow
