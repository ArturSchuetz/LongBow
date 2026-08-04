#include <DirectX11RenderDevice/BowD3D11RenderDevice.h>

#include <DirectX11RenderDevice/Device/BowD3D11GraphicsWindow.h>

#include <CoreSystems/BowLogger.h>
#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <RenderDevice/Device/Shader/Textures/IBowTexture2D.h>

#include <dxgi1_3.h>

namespace bow
{

D3D11RenderDevice::D3D11RenderDevice() : m_featureLevel(D3D_FEATURE_LEVEL_11_0), m_initialized(false) { FN("D3D11RenderDevice::D3D11RenderDevice"); }

D3D11RenderDevice::~D3D11RenderDevice()
{
    FN("D3D11RenderDevice::~D3D11RenderDevice");

    VRelease();
}

bool D3D11RenderDevice::Initialize(uint32_t deviceHandle)
{
    FN("D3D11RenderDevice::Initialize");

    UINT factoryFlags = 0;
    UINT deviceFlags = 0;

#ifdef _DEBUG
    // The debug layer turns silent misuse into messages in the debug output,
    // and is the DirectX counterpart of the OpenGL debug callback.
    factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory));
    if (FAILED(result))
    {
#ifdef _DEBUG
        // A machine without the graphics tools optional feature has no debug
        // layer, which is not a reason to fail.
        LOG_WARNING("Could not create a debug DXGI factory, retrying without it.");
        result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
        deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
#endif
        if (FAILED(result))
        {
            LOG_ERROR("CreateDXGIFactory2 failed (0x%08X).", (unsigned)result);
            return false;
        }
    }

    // deviceHandle selects the adapter; 0 means "let DXGI pick", which is the
    // default adapter rather than a specific one.
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    if (deviceHandle != 0)
    {
        if (FAILED(m_factory->EnumAdapters1(deviceHandle, &adapter)))
        {
            LOG_WARNING("No adapter with index %u, falling back to the default one.", deviceHandle);
            adapter.Reset();
        }
    }

    static const D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};

    // The driver type has to be UNKNOWN when an adapter is given, and HARDWARE
    // when it is not; passing the wrong one is rejected outright.
    const D3D_DRIVER_TYPE driverType = (adapter != nullptr) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;

    result = D3D11CreateDevice(adapter.Get(), driverType, nullptr, deviceFlags, featureLevels, (UINT)(sizeof(featureLevels) / sizeof(featureLevels[0])), D3D11_SDK_VERSION, &m_device, &m_featureLevel, &m_deviceContext);

    if (FAILED(result) && (deviceFlags & D3D11_CREATE_DEVICE_DEBUG) != 0)
    {
        LOG_WARNING("Could not create a debug device, retrying without the debug layer.");
        deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
        result = D3D11CreateDevice(adapter.Get(), driverType, nullptr, deviceFlags, featureLevels, (UINT)(sizeof(featureLevels) / sizeof(featureLevels[0])), D3D11_SDK_VERSION, &m_device, &m_featureLevel, &m_deviceContext);
    }

    if (FAILED(result))
    {
        LOG_ERROR("D3D11CreateDevice failed (0x%08X).", (unsigned)result);
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    if (SUCCEEDED(m_device.As(&dxgiDevice)))
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> usedAdapter;
        if (SUCCEEDED(dxgiDevice->GetAdapter(&usedAdapter)))
        {
            DXGI_ADAPTER_DESC description = {};
            if (SUCCEEDED(usedAdapter->GetDesc(&description)))
            {
                char name[128] = {0};
                WideCharToMultiByte(CP_UTF8, 0, description.Description, -1, name, sizeof(name) - 1, nullptr, nullptr);
                LOG_INFO("DirectX 11 feature level %u.%u on %s", (unsigned)(m_featureLevel >> 12) & 0xF, (unsigned)(m_featureLevel >> 8) & 0xF, name);
            }
        }
    }

    m_initialized = true;
    return true;
}

void D3D11RenderDevice::VRelease()
{
    FN("D3D11RenderDevice::VRelease");

    if (!m_initialized)
    {
        return;
    }

    m_deviceContext.Reset();
    m_device.Reset();
    m_factory.Reset();

    m_initialized = false;
    LOG_TRACE("D3D11RenderDevice released");
}

GraphicsWindowPtr D3D11RenderDevice::VCreateWindow(int width, int height, const std::string &title, WindowType type)
{
    FN("D3D11RenderDevice::VCreateWindow");

    D3D11GraphicsWindowPtr window = D3D11GraphicsWindowPtr(new D3D11GraphicsWindow());
    if (!window->Initialize((uint32_t)width, (uint32_t)height, title, type, this))
    {
        LOG_ERROR("Could not create a DirectX 11 window.");
        return nullptr;
    }

    return window;
}

//
// The resource types below are still to be written. Each one says which piece
// is missing rather than handing back a null pointer for the caller to trip
// over later.
//

namespace
{
void ReportMissing(const char *what)
{
    LOG_ERROR("DirectX 11: %s is not implemented yet.", what);
}
} // namespace

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgramFromFile(const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgramFromFile");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgramFromFile(const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgramFromFile");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgramFromFile(const std::string &, const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgramFromFile");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgramFromFile(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgramFromFile");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgram(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgram");
    ReportMissing("shader programs");
    return nullptr;
}

std::unique_ptr<IRayTracingShaderProgram> D3D11RenderDevice::VCreateRayTracingShaderProgram(const std::string &, const std::string &, const std::string &, const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateRayTracingShaderProgram");

    LOG_ERROR("Ray tracing is not supported in DirectX 11.");
    return nullptr;
}

ComputeShaderProgramPtr D3D11RenderDevice::VCreateComputeShaderProgram(const std::string &)
{
    FN("D3D11RenderDevice::VCreateComputeShaderProgram");
    ReportMissing("compute shaders");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgram(const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgram");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgram(const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgram");
    ReportMissing("shader programs");
    return nullptr;
}

ShaderProgramPtr D3D11RenderDevice::VCreateShaderProgram(const std::string &, const std::string &, const std::string &, const std::string &)
{
    FN("D3D11RenderDevice::VCreateShaderProgram");
    ReportMissing("shader programs");
    return nullptr;
}

MeshBufferPtr D3D11RenderDevice::VCreateMeshBuffers(MeshAttribute, ShaderVertexAttributeMap, BufferHint)
{
    FN("D3D11RenderDevice::VCreateMeshBuffers");
    ReportMissing("mesh buffers");
    return nullptr;
}

VertexBufferPtr D3D11RenderDevice::VCreateVertexBuffer(BufferHint, int, bool)
{
    FN("D3D11RenderDevice::VCreateVertexBuffer");
    ReportMissing("vertex buffers");
    return nullptr;
}

IndexBufferPtr D3D11RenderDevice::VCreateIndexBuffer(BufferHint, IndexBufferDatatype, int, bool)
{
    FN("D3D11RenderDevice::VCreateIndexBuffer");
    ReportMissing("index buffers");
    return nullptr;
}

WritePixelBufferPtr D3D11RenderDevice::VCreateWritePixelBuffer(PixelBufferHint, int)
{
    FN("D3D11RenderDevice::VCreateWritePixelBuffer");
    ReportMissing("pixel buffers");
    return nullptr;
}

UniformBufferPtr D3D11RenderDevice::VCreateUniformBuffer(BufferHint, int, void *)
{
    FN("D3D11RenderDevice::VCreateUniformBuffer");
    ReportMissing("uniform buffers");
    return nullptr;
}

StorageBufferPtr D3D11RenderDevice::VCreateStorageBuffer(BufferHint, int, void *)
{
    FN("D3D11RenderDevice::VCreateStorageBuffer");
    ReportMissing("storage buffers");
    return nullptr;
}

Texture2DPtr D3D11RenderDevice::VCreateTexture2D(Texture2DDescription)
{
    FN("D3D11RenderDevice::VCreateTexture2D");
    ReportMissing("textures");
    return nullptr;
}

Texture2DPtr D3D11RenderDevice::VCreateTexture2D(ImagePtr)
{
    FN("D3D11RenderDevice::VCreateTexture2D");
    ReportMissing("textures");
    return nullptr;
}

TextureSamplerPtr D3D11RenderDevice::VCreateTexture2DSampler(TextureMinificationFilter, TextureMagnificationFilter, TextureWrap, TextureWrap, float)
{
    FN("D3D11RenderDevice::VCreateTexture2DSampler");
    ReportMissing("texture samplers");
    return nullptr;
}

std::unique_ptr<IBottomLevelAccelerationStructure> D3D11RenderDevice::VCreateBottomLevelAccelerationStructure(MeshPtr)
{
    FN("D3D11RenderDevice::VCreateBottomLevelAccelerationStructure");

    LOG_ERROR("Ray tracing is not supported in DirectX 11.");
    return nullptr;
}

std::unique_ptr<ITopLevelAccelerationStructure> D3D11RenderDevice::VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &)
{
    FN("D3D11RenderDevice::VCreateTopLevelAccelerationStructure");

    LOG_ERROR("Ray tracing is not supported in DirectX 11.");
    return nullptr;
}

} // namespace bow
