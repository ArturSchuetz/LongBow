#include <CoreSystems/BowCorePredeclares.h>
#include <CoreSystems/BowLogger.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>
#include <CoreSystems/Geometry/Indices/BowIndicesUnsignedInt.h>
#include <CoreSystems/Geometry/Indices/BowIndicesUnsignedShort.h>
#include <CoreSystems/Geometry/Indices/IBowIndicesBase.h>
#include <CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h>

#include <Resources/BowResourcesPredeclares.h>
#include <Resources/Resources/BowImage.h>
#include <Resources/Resources/BowMesh.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/Mesh/BowMeshBuffers.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

#include <OpenGL3xRenderDevice/BowOGL3xRenderDevice.h>
#include <OpenGL3xRenderDevice/BowOGL3xTypeConverter.h>
#include <OpenGL3xRenderDevice/Device/BowOGL3xGraphicsWindow.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xIndexBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xStorageBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xUniformBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xVertexBuffer.h>
#include <OpenGL3xRenderDevice/Device/Buffer/BowOGL3xWritePixelBuffer.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xComputeShaderProgram.h>
#include <OpenGL3xRenderDevice/Device/Shader/BowOGL3xShaderProgram.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTexture2D.h>
#include <OpenGL3xRenderDevice/Device/Textures/BowOGL3xTextureSampler.h>

#include <optick.h>

#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

namespace bow
{

void error_callback(int /*error*/, const char *description)
{
    FN("error_callback");
    LOG_ERROR(description);
}

OGLRenderDevice::OGLRenderDevice() : m_maximumNumberOfVertexAttributes(0), m_numberOfTextureUnits(0), m_maximumNumberOfColorAttachments(0) { FN("OGLRenderDevice::OGLRenderDevice"); }

OGLRenderDevice::~OGLRenderDevice()
{
    FN("OGLRenderDevice::~OGLRenderDevice");

    VRelease();
}

bool OGLRenderDevice::Initialize()
{
    FN("OGLRenderDevice::Initialize");

    LOG_TRACE("glfwSetErrorCallback");
    glfwSetErrorCallback(error_callback);
    LOG_TRACE("glfwInit");
    if (GL_TRUE != glfwInit())
    {
        LOG_ERROR("Could not initialize GLFW!");
        return false;
    }
    else
    {
        LOG_TRACE("GLFW sucessfully initialized!");
    }
    return true;
}

void OGLRenderDevice::VRelease()
{
    FN("OGLRenderDevice::VRelease");

    LOG_TRACE("glfwTerminate");
    glfwTerminate();
}

GraphicsWindowPtr OGLRenderDevice::VCreateWindow(int width, int height, const std::string &title, WindowType type)
{
    FN("OGLRenderDevice::VCreateWindow");
    OPTICK_EVENT();

    OGLGraphicsWindowPtr pGraphicsWindow = OGLGraphicsWindowPtr(new OGLGraphicsWindow());
    if (pGraphicsWindow->Initialize(width, height, title, type, this))
    {
        LOG_TRACE("glGetIntegerv");
        glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &m_maximumNumberOfVertexAttributes);
        LOG_TRACE("\tMaximum number of Vertex Attributes: %i", m_maximumNumberOfVertexAttributes);

        LOG_TRACE("glGetIntegerv");
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_numberOfTextureUnits);
        LOG_TRACE("\tMaximum number of Texture Units: %i", m_numberOfTextureUnits);

        LOG_TRACE("glGetIntegerv");
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maximumNumberOfColorAttachments);
        LOG_TRACE("\tMaximum number of Color Attachments: %i", m_maximumNumberOfColorAttachments);

        return pGraphicsWindow;
    }
    else
    {
        LOG_ERROR("Error while creating OpenGL-Window!");
        return GraphicsWindowPtr(nullptr);
    }
}

// =========================================================================
// SHADER STUFF:
// =========================================================================
ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename)
{
    FN("OGLRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    // Open file
    std::string vshaderString;
    std::ifstream vsourceFile(VertexShaderFilename.c_str());

    // Source file loaded
    if (vsourceFile)
    {
        // Get shader source
        vshaderString.assign((std::istreambuf_iterator<char>(vsourceFile)), std::istreambuf_iterator<char>());

        // Open file
        std::string fshaderString;
        std::ifstream fsourceFile(FragementShaderFilename.c_str());

        if (fsourceFile)
        {
            // Get shader source
            fshaderString.assign((std::istreambuf_iterator<char>(fsourceFile)), std::istreambuf_iterator<char>());
            return VCreateShaderProgram(vshaderString, fshaderString);
        }
        else
        {
            LOG_ERROR("Could not open Shader from File");
            return OGLShaderProgramPtr(nullptr);
        }
    }
    else
    {
        LOG_ERROR("Could not open Shader from File");
        return OGLShaderProgramPtr(nullptr);
    }
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename)
{
    FN("OGLRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    // Open file
    std::string vshaderString;
    std::ifstream vsourceFile(VertexShaderFilename.c_str());

    // Source file loaded
    if (vsourceFile)
    {
        // Get shader source
        vshaderString.assign((std::istreambuf_iterator<char>(vsourceFile)), std::istreambuf_iterator<char>());

        // Open file
        std::string gshaderString;
        std::ifstream gsourceFile(GeometryShaderFilename.c_str());

        if (gsourceFile)
        {
            // Get shader source
            gshaderString.assign((std::istreambuf_iterator<char>(gsourceFile)), std::istreambuf_iterator<char>());

            // Open file
            std::string fshaderString;
            std::ifstream fsourceFile(FragementShaderFilename.c_str());

            if (fsourceFile)
            {
                // Get shader source
                fshaderString.assign((std::istreambuf_iterator<char>(fsourceFile)), std::istreambuf_iterator<char>());
                return VCreateShaderProgram(vshaderString, gshaderString, fshaderString);
            }
            else
            {
                LOG_ERROR("Could not open Shader from File");
                return OGLShaderProgramPtr(nullptr);
            }
        }
        else
        {
            LOG_ERROR("Could not open Shader from File");
            return OGLShaderProgramPtr(nullptr);
        }
    }
    else
    {
        LOG_ERROR("Could not open Shader from File");
        return OGLShaderProgramPtr(nullptr);
    }
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename)
{
    FN("OGLRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    // Open file
    std::string vshaderString;
    std::ifstream vsourceFile(VertexShaderFilename.c_str());

    // Source file loaded
    if (vsourceFile)
    {
        // Get shader source
        vshaderString.assign((std::istreambuf_iterator<char>(vsourceFile)), std::istreambuf_iterator<char>());

        // Open file
        std::string fshaderString;
        std::ifstream fsourceFile(FragmentShaderFilename.c_str());

        if (fsourceFile)
        {
            // Get shader source
            fshaderString.assign((std::istreambuf_iterator<char>(fsourceFile)), std::istreambuf_iterator<char>());

            // Open file
            std::string tcshaderString;
            std::ifstream tcsourceFile(TessControlShaderFilename.c_str());

            if (tcsourceFile)
            {
                // Get shader source
                tcshaderString.assign((std::istreambuf_iterator<char>(tcsourceFile)), std::istreambuf_iterator<char>());

                // Open file
                std::string teshaderString;
                std::ifstream tesourceFile(TessEvalShaderFilename.c_str());

                if (tesourceFile)
                {
                    // Get shader source
                    teshaderString.assign((std::istreambuf_iterator<char>(tesourceFile)), std::istreambuf_iterator<char>());
                    return VCreateShaderProgram(vshaderString, fshaderString, tcshaderString, teshaderString);
                }
                else
                {
                    LOG_ERROR("Could not open Shader from File");
                    return OGLShaderProgramPtr(nullptr);
                }
            }
            else
            {
                LOG_ERROR("Could not open Shader from File");
                return OGLShaderProgramPtr(nullptr);
            }
        }
        else
        {
            LOG_ERROR("Could not open Shader from File");
            return OGLShaderProgramPtr(nullptr);
        }
    }
    else
    {
        LOG_ERROR("Could not open Shader from File");
        return OGLShaderProgramPtr(nullptr);
    }
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                               const std::string &TessEvalShaderFilename)
{
    FN("OGLRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    // Open file
    std::string vshaderString;
    std::ifstream vsourceFile(VertexShaderFilename.c_str());

    // Source file loaded
    if (vsourceFile)
    {
        // Get shader source
        vshaderString.assign((std::istreambuf_iterator<char>(vsourceFile)), std::istreambuf_iterator<char>());

        // Open file
        std::string gshaderString;
        std::ifstream gsourceFile(GeometryShaderFilename.c_str());

        if (gsourceFile)
        {
            // Get shader source
            gshaderString.assign((std::istreambuf_iterator<char>(gsourceFile)), std::istreambuf_iterator<char>());

            // Open file
            std::string fshaderString;
            std::ifstream fsourceFile(FragmentShaderFilename.c_str());

            if (fsourceFile)
            {
                // Get shader source
                fshaderString.assign((std::istreambuf_iterator<char>(fsourceFile)), std::istreambuf_iterator<char>());

                // Open file
                std::string tcshaderString;
                std::ifstream tcsourceFile(TessControlShaderFilename.c_str());

                if (tcsourceFile)
                {
                    // Get shader source
                    tcshaderString.assign((std::istreambuf_iterator<char>(tcsourceFile)), std::istreambuf_iterator<char>());

                    // Open file
                    std::string teshaderString;
                    std::ifstream tesourceFile(TessEvalShaderFilename.c_str());

                    if (tesourceFile)
                    {
                        // Get shader source
                        teshaderString.assign((std::istreambuf_iterator<char>(tesourceFile)), std::istreambuf_iterator<char>());
                        return VCreateShaderProgram(vshaderString, gshaderString, fshaderString, tcshaderString, teshaderString);
                    }
                    else
                    {
                        LOG_ERROR("Could not open Shader from File");
                        return OGLShaderProgramPtr(nullptr);
                    }
                }
                else
                {
                    LOG_ERROR("Could not open Shader from File");
                    return OGLShaderProgramPtr(nullptr);
                }
            }
            else
            {
                LOG_ERROR("Could not open Shader from File");
                return OGLShaderProgramPtr(nullptr);
            }
        }
        else
        {
            LOG_ERROR("Could not open Shader from File");
            return OGLShaderProgramPtr(nullptr);
        }
    }
    else
    {
        LOG_ERROR("Could not open Shader from File");
        return OGLShaderProgramPtr(nullptr);
    }
}

ComputeShaderProgramPtr OGLRenderDevice::VCreateComputeShaderProgram(const std::string &computeShaderSource)
{
    FN("OGLRenderDevice::VCreateComputeShaderProgram");
    OPTICK_EVENT();

    return OGLComputeShaderProgramPtr(new OGLComputeShaderProgram(computeShaderSource));
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragementShaderSource)
{
    FN("OGLRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, std::string(), FragementShaderSource, std::string(), std::string());
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragementShaderSource)
{
    FN("OGLRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return OGLShaderProgramPtr(new OGLShaderProgram(VertexShaderSource, GeometryShaderSource, FragementShaderSource, std::string(), std::string()));
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource)
{
    FN("OGLRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return OGLShaderProgramPtr(new OGLShaderProgram(VertexShaderSource, std::string(), FragmentShaderSource, TessControlShaderSource, TessEvalShaderSource));
}

ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource,
                                                       const std::string &TessEvalShaderSource)
{
    FN("OGLRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return OGLShaderProgramPtr(new OGLShaderProgram(VertexShaderSource, GeometryShaderSource, FragmentShaderSource, TessControlShaderSource, TessEvalShaderSource));
}

MeshBufferPtr OGLRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
{
    FN("OGLRenderDevice::VCreateMeshBuffers");
    OPTICK_EVENT();

    MeshBuffers *meshBuffers = new MeshBuffers();

    if (mesh.Indices != nullptr)
    {
        if (mesh.Indices->Type == IndicesType::UnsignedShort)
        {
            std::vector<uint16_t> meshIndices = (std::dynamic_pointer_cast<IndicesUnsignedShort>(mesh.Indices))->Values;

            std::vector<uint16_t> indices = std::vector<uint16_t>(meshIndices.size());
            for (size_t j = 0; j < meshIndices.size(); ++j)
            {
                indices[j] = meshIndices[j];
            }

            IndexBufferPtr indexBuffer = VCreateIndexBuffer(usageHint, IndexBufferDatatype::UnsignedInt16, indices.size() * sizeof(uint16_t));
            indexBuffer->VCopyFromSystemMemory(&(indices[0]), indices.size() * sizeof(uint16_t));
            meshBuffers->IndexBuffer = indexBuffer;
        }
        else if (mesh.Indices->Type == IndicesType::UnsignedInt)
        {
            std::vector<uint32_t> meshIndices = (std::dynamic_pointer_cast<IndicesUnsignedInt>(mesh.Indices))->Values;

            std::vector<uint32_t> indices = std::vector<uint32_t>(meshIndices.size());
            for (size_t j = 0; j < meshIndices.size(); ++j)
            {
                indices[j] = meshIndices[j];
            }

            IndexBufferPtr indexBuffer = VCreateIndexBuffer(usageHint, IndexBufferDatatype::UnsignedInt32, indices.size() * sizeof(uint32_t));
            indexBuffer->VCopyFromSystemMemory(&(indices[0]), indices.size() * sizeof(uint32_t));
            meshBuffers->IndexBuffer = indexBuffer;
        }
        else
        {
            LOG_ASSERT(false, "mesh.Indices.Datatype is not supported.");
        }
    }

    for (auto shaderAttribute = shaderAttributes.begin(); shaderAttribute != shaderAttributes.end(); ++shaderAttribute)
    {
        VertexAttributePtr attribute = mesh.GetAttribute(shaderAttribute->second->Name);
        if (attribute.get() == nullptr)
        {
            LOG_ERROR("Shader requires vertex attribute \"%s\", which is not "
                      "present in mesh.",
                      shaderAttribute->first);
        }

        if (attribute->Type == VertexAttributeType::UnsignedByte)
        {
            uint32_t count = (std::dynamic_pointer_cast<VertexAttribute<uint8_t>>(attribute))->Values.size();

            VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(uint8_t) * count);
            vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<uint8_t>>(attribute))->Values[0]), 0, sizeof(uint8_t) * count);

            meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::UnsignedByte, 1)));
        }
        else if (attribute->Type == VertexAttributeType::Float)
        {
            uint32_t count = (std::dynamic_pointer_cast<VertexAttribute<float>>(attribute))->Values.size();

            VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(float) * count);
            vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<float>>(attribute))->Values[0]), 0, sizeof(float) * count);

            meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 1)));
        }
        else if (attribute->Type == VertexAttributeType::FloatVector2)
        {
            uint32_t count = (std::dynamic_pointer_cast<VertexAttribute<Vector2<float>>>(attribute))->Values.size();

            VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector2<float>) * count);
            vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector2<float>>>(attribute))->Values[0]), 0, sizeof(Vector2<float>) * count);

            meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 2)));
        }
        else if (attribute->Type == VertexAttributeType::FloatVector3)
        {
            uint32_t count = (std::dynamic_pointer_cast<VertexAttribute<Vector3<float>>>(attribute))->Values.size();

            VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector3<float>) * count);
            vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector3<float>>>(attribute))->Values[0]), 0, sizeof(Vector3<float>) * count);

            meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 3)));
        }
        else if (attribute->Type == VertexAttributeType::FloatVector4)
        {
            uint32_t count = (std::dynamic_pointer_cast<VertexAttribute<Vector4<float>>>(attribute))->Values.size();

            VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector4<float>) * count);
            vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector4<float>>>(attribute))->Values[0]), 0, sizeof(Vector4<float>) * count);

            meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 4)));
        }
        else
        {
            LOG_ERROR("attribute.Datatype not implemented!");
        }
    }

    return MeshBufferPtr(meshBuffers);
}

VertexBufferPtr OGLRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
{
    FN("OGLRenderDevice::VCreateVertexBuffer");
    OPTICK_EVENT();

    return OGLVertexBufferPtr(new OGLVertexBuffer(usageHint, sizeInBytes));
}

IndexBufferPtr OGLRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
{
    FN("OGLRenderDevice::VCreateIndexBuffer");
    OPTICK_EVENT();

    return OGLIndexBufferPtr(new OGLIndexBuffer(usageHint, dataType, sizeInBytes));
}

UniformBufferPtr OGLRenderDevice::VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("OGLRenderDevice::VCreateUniformBuffer");
    OPTICK_EVENT();

    OGLShaderResourceBufferPtr uniformBuffer = OGLShaderResourceBufferPtr(new OGLShaderResourceBuffer(usageHint, sizeInBytes));
    if (data != nullptr)
    {
        uniformBuffer->VCopyFromSystemMemory(data, 0, sizeInBytes);
    }
    return uniformBuffer;
}

StorageBufferPtr OGLRenderDevice::VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("OGLRenderDevice::CreateShaderStorageBuffer");
    OPTICK_EVENT();

    OGLStorageBufferPtr shaderStorageBuffer = OGLStorageBufferPtr(new OGLStorageBuffer(usageHint, sizeInBytes));
    if (data != nullptr)
    {
        shaderStorageBuffer->VCopyFromSystemMemory(data, 0, sizeInBytes);
    }
    return shaderStorageBuffer;
}

WritePixelBufferPtr OGLRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
{
    FN("OGLRenderDevice::VCreateWritePixelBuffer");
    OPTICK_EVENT();

    return OGLWritePixelBufferPtr(new OGLWritePixelBuffer(usageHint, sizeInBytes));
}

Texture2DPtr OGLRenderDevice::VCreateTexture2D(Texture2DDescription description)
{
    FN("OGLRenderDevice::VCreateTexture2D");
    OPTICK_EVENT();

    return OGLTexture2DPtr(new OGLTexture2D(description, GL_TEXTURE_2D));
}

Texture2DPtr OGLRenderDevice::VCreateTexture2D(ImagePtr image)
{
    FN("OGLRenderDevice::VCreateTexture2D");
    OPTICK_EVENT();

    if (image->VGetSizeInBytes() == 0)
        return Texture2DPtr(nullptr);

    ImageDatatype datatype;
    TextureFormat format;
    switch (image->GetFormat())
    {
    case Image::Format::uchar:
        datatype = ImageDatatype::UnsignedByte;
        if (image->GetNumChannels() == 3)
            format = TextureFormat::RedGreenBlue8;
        else if (image->GetNumChannels() == 4)
            format = TextureFormat::RedGreenBlueAlpha8;
        else
        {
            LOG_ERROR("Image format not supported!");
            return Texture2DPtr(nullptr);
        }
        break;
    case Image::Format::float32:
        datatype = ImageDatatype::Float;
        if (image->GetNumChannels() == 3)
            format = TextureFormat::RedGreenBlue32f;
        else if (image->GetNumChannels() == 4)
            format = TextureFormat::RedGreenBlueAlpha32f;
        else
        {
            LOG_ERROR("Image format not supported!");
            return Texture2DPtr(nullptr);
        }
        break;
    default:
        LOG_ERROR("Image format not supported!");
        return Texture2DPtr(nullptr);
    }

    Texture2DPtr texture = OGLTexture2DPtr(new OGLTexture2D(Texture2DDescription(image->GetWidth(), image->GetHeight(), format, true), GL_TEXTURE_2D));
    texture->VCopyFromSystemMemory(image->GetData(), OGLTypeConverter::TextureToImageFormat(format), datatype);
    return texture;
}

TextureSamplerPtr OGLRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
{
    FN("OGLRenderDevice::VCreateTexture2DSampler");
    OPTICK_EVENT();

    return OGLTextureSamplerPtr(new OGLTextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy));
}

std::unique_ptr<IRayTracingShaderProgram> OGLRenderDevice::VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                                          const std::string &intersectionShaderSource, const std::string &callableShaderSource)
{
    FN("OGLRenderDevice::VCreateRayTracingShaderProgram");
    OPTICK_EVENT();

    LOG_FATAL("Ray Tracing is not supported in OpenGL!");

    return nullptr;
}

std::unique_ptr<IBottomLevelAccelerationStructure> OGLRenderDevice::VCreateBottomLevelAccelerationStructure(VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings)
{
    FN("OGLRenderDevice::VCreateBottomLevelAccelerationStructure");
    OPTICK_EVENT();

    LOG_FATAL("Ray Tracing is not supported in OpenGL!");

    return nullptr;
}
std::unique_ptr<IBottomLevelAccelerationStructure> OGLRenderDevice::VCreateBottomLevelAccelerationStructure(uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings)
{
    FN("OGLRenderDevice::VCreateBottomLevelAccelerationStructure");
    OPTICK_EVENT();

    VulkanVertexAttributeBindingsPtr vulkanVertexAttributeBindings = std::dynamic_pointer_cast<VulkanVertexAttributeBindings>(vertexAttributeBindings);
    LOG_FATAL("Ray Tracing is not supported in OpenGL!");

    return nullptr;
}

std::unique_ptr<ITopLevelAccelerationStructure> OGLRenderDevice::VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures)
{
    FN("OGLRenderDevice::VCreateTopLevelAccelerationStructure");
    OPTICK_EVENT();

    LOG_FATAL("Ray Tracing is not supported in OpenGL!");

    return nullptr;
}

} // namespace bow