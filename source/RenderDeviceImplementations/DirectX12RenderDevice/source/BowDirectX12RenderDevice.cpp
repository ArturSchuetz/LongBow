#include <DirectX12RenderDevice/BowDirectX12RenderDevice.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/Device/Context/Mesh/BowMeshBuffers.h>
#include <RenderDevice/Device/Context/VertexAttributeBindings/BowVertexBufferAttribute.h>
#include <RenderDevice/Device/Shader/BowShaderVertexAttribute.h>
#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>
#include <RenderDevice/Device/Shader/Textures/IBowTextureSampler.h>

#include <Resources/BowResourcesPredeclares.h>
#include <Resources/Resources/BowImage.h>
#include <Resources/Resources/BowMesh.h>

#include <CoreSystems/BowCorePredeclares.h>
#include <CoreSystems/BowLogger.h>

#include <CoreSystems/BowUtils.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>
#include <CoreSystems/Geometry/Indices/BowIndicesUnsignedInt.h>
#include <CoreSystems/Geometry/Indices/BowIndicesUnsignedShort.h>
#include <CoreSystems/Geometry/Indices/IBowIndicesBase.h>
#include <CoreSystems/Geometry/VertexAttributes/IBowVertexAttribute.h>

#include <optick.h>

namespace bow
{

uint32_t g_DeviceCount = 0;

void error_callback(int /*errorCode*/, const char *description)
{
    FN("error_callback");
    LOG_ERROR(description);
}

DirectX12RenderDevice::DirectX12RenderDevice()
{
    FN("DirectX12RenderDevice::DirectX12RenderDevice");

    g_DeviceCount++;
}

DirectX12RenderDevice::~DirectX12RenderDevice()
{
    FN("DirectX12RenderDevice::~DirectX12RenderDevice");

    g_DeviceCount--;
    VRelease();
}

bool DirectX12RenderDevice::Initialize(int deviceHandle)
{
    FN("DirectX12RenderDevice::Initialize");

    if (g_DeviceCount > 0)
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to initialize COM.");
            return false;
        }
    }

    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create D3D12 device.");
        return false;
    }

    return true; // Initialization successful.
}

void DirectX12RenderDevice::VRelease()
{
    FN("DirectX12RenderDevice::VRelease");

    if (g_DeviceCount == 0)
    {
        CoUninitialize();

        LOG_TRACE("DirectX12RenderDevice released");
    }
}

GraphicsWindowPtr DirectX12RenderDevice::VCreateWindow(int width, int height, const std::string &title, WindowType type)
{
    FN("DirectX12RenderDevice::VCreateWindow");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename)
{
    FN("DirectX12RenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, std::string(), FragementShaderFilename);
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename)
{
    FN("DirectX12RenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, GeometryShaderFilename, FragementShaderFilename, std::string(), std::string());
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename)
{
    FN("DirectX12RenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, FragmentShaderFilename, std::string(), TessControlShaderFilename, TessEvalShaderFilename);
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                                     const std::string &TessEvalShaderFilename)
{
    FN("DirectX12RenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

ComputeShaderProgramPtr DirectX12RenderDevice::VCreateComputeShaderProgram(const std::string &computeShaderSource)
{
    FN("DirectX12RenderDevice::VCreateComputeShaderProgram");
    OPTICK_EVENT();

    throw std::runtime_error("The method or operation is not implemented.");
    return nullptr;
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource)
{
    FN("DirectX12RenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, std::string());
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource)
{
    FN("DirectX12RenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, GeometryShaderSource, std::string(), std::string());
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource)
{
    FN("DirectX12RenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, std::string(), TessControlShaderSource, TessEvalShaderSource);
}

ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                                             const std::string &TessEvalShaderSource)
{
    FN("DirectX12RenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

MeshBufferPtr DirectX12RenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
{
    FN("DirectX12RenderDevice::VCreateMeshBuffers");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

VertexBufferPtr DirectX12RenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes, bool useForRayTracing)
{
    FN("DirectX12RenderDevice::VCreateVertexBuffer");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

IndexBufferPtr DirectX12RenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes, bool useForRayTracing)
{
    FN("DirectX12RenderDevice::VCreateIndexBuffer");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

WritePixelBufferPtr DirectX12RenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
{
    FN("DirectX12RenderDevice::VCreateWritePixelBuffer");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

UniformBufferPtr DirectX12RenderDevice::VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("DirectX12RenderDevice::VCreateUniformBuffer");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

StorageBufferPtr DirectX12RenderDevice::VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("DirectX12RenderDevice::VCreateStorageBuffer");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

Texture2DPtr DirectX12RenderDevice::VCreateTexture2D(Texture2DDescription description)
{
    FN("DirectX12RenderDevice::VCreateTexture2D");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

Texture2DPtr DirectX12RenderDevice::VCreateTexture2D(ImagePtr image)
{
    FN("DirectX12RenderDevice::VCreateTexture2D");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

TextureSamplerPtr DirectX12RenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
{
    FN("DirectX12RenderDevice::VCreateTexture2DSampler");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

std::unique_ptr<IRayTracingShaderProgram> DirectX12RenderDevice::VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource,
                                                                                                const std::string &missShaderSource, const std::string &intersectionShaderSource, const std::string &callableShaderSource)
{
    FN("DirectX12RenderDevice::VCreateRayTracingShaderProgram");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

std::unique_ptr<IBottomLevelAccelerationStructure> DirectX12RenderDevice::VCreateBottomLevelAccelerationStructure(MeshPtr mesh)
{
    FN("DirectX12RenderDevice::VCreateBottomLevelAccelerationStructure");

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

std::unique_ptr<ITopLevelAccelerationStructure> DirectX12RenderDevice::VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures)
{
    FN("DirectX12RenderDevice::VCreateTopLevelAccelerationStructure");
    OPTICK_EVENT();

    LOG_FATAL("Not yet implemented!");
    return nullptr;
}

// =================================================================================================
// PRIVATE METHODS
// =================================================================================================

} // namespace bow