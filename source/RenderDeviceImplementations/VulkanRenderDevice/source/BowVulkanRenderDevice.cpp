#include <VulkanRenderDevice/BowVulkanRenderDevice.h>

#include <VulkanRenderDevice/BowVulkanInstance.h>
#include <VulkanRenderDevice/BowVulkanPhysicalDevice.h>
#include <VulkanRenderDevice/BowVulkanQueueFamily.h>
#include <VulkanRenderDevice/BowVulkanTypeConverter.h>
#include <VulkanRenderDevice/Device/BowVulkanGraphicsWindow.h>
#include <VulkanRenderDevice/Device/BowVulkanLogicalDevice.h>
#include <VulkanRenderDevice/Device/BowVulkanRenderSurface.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanDeviceMemory.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanIndexBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanStorageBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanUniformBuffer.h>
#include <VulkanRenderDevice/Device/Buffer/BowVulkanVertexBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanCommandBuffer.h>
#include <VulkanRenderDevice/Device/CommandPool/BowVulkanSingleUseCommandBuffer.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanBottomLevelAccelerationStructure.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanRayTracingShaderProgram.h>
#include <VulkanRenderDevice/Device/RayTracing/BowVulkanTopLevelAccelerationStructure.h>
#include <VulkanRenderDevice/Device/Shader/BowVulkanShaderProgram.h>
#include <VulkanRenderDevice/Device/Surface/VertexAttributeBindings/BowVulkanVertexAttributeBindings.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTexture2D.h>
#include <VulkanRenderDevice/Device/Textures/BowVulkanTextureSampler.h>

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

VulkanInstancePtr g_pVulkanInstance = nullptr;
uint32_t g_DeviceCount = 0;

void error_callback(int /*errorCode*/, const char *description)
{
    FN("error_callback");
    LOG_ERROR(description);
}

VulkanRenderDevice::VulkanRenderDevice()
    : m_physicalDevices(), m_selectedPhysicalDevice(nullptr), m_logicalDevice(nullptr), m_graphicsQueueFamilyIdx(UINT32_MAX), m_presentQueueFamilyIdx(UINT32_MAX), m_computeQueueFamilyIndex(UINT32_MAX), m_transferQueueFamilyIndex(UINT32_MAX),
      m_guid(Utils::GenerateGUID())
{
    FN("VulkanRenderDevice::VulkanRenderDevice");

    g_DeviceCount++;
}

VulkanRenderDevice::~VulkanRenderDevice()
{
    FN("VulkanRenderDevice::~VulkanRenderDevice");

    g_DeviceCount--;
    VRelease();
}

bool VulkanRenderDevice::Initialize(int deviceHandle)
{
    FN("VulkanRenderDevice::Initialize");

    // Ensure there is a Vulkan instance available, initialize if not already
    // done.
    if (g_pVulkanInstance == nullptr)
    {
        g_pVulkanInstance = VulkanInstancePtr(new VulkanInstance());
        if (!g_pVulkanInstance->Initialize())
        {
            return false; // Initialization failed.
        }
    }

    // Retrieve the count of physical devices available.
    uint32_t gpu_count = 1;

    LOG_TRACE("vkEnumeratePhysicalDevices %s", m_guid.c_str());
    VkResult enumeration_res = vkEnumeratePhysicalDevices(g_pVulkanInstance->GetHandle(), &gpu_count, nullptr);

    if (enumeration_res != VK_SUCCESS || gpu_count == 0)
    {
        if (gpu_count == 0)
            LOG_ERROR("VulkanRenderDevice: No Supported Physical Devices");
        else
            LOG_ERROR("VulkanRenderDevice: %s", VulkanTypeConverter::ToString(enumeration_res).c_str());
        return false; // No GPUs found or error during enumeration.
    }

    // Store all available physical devices.
    std::vector<VkPhysicalDevice> physical_devices(gpu_count);
    LOG_TRACE("vkEnumeratePhysicalDevices %s", m_guid.c_str());
    enumeration_res = vkEnumeratePhysicalDevices(g_pVulkanInstance->GetHandle(), &gpu_count, &physical_devices[0]);

    if (enumeration_res != VK_SUCCESS || gpu_count == 0)
    {
        if (gpu_count == 0)
            LOG_ERROR("VulkanRenderDevice: %s", VulkanTypeConverter::ToString(enumeration_res).c_str());
        else
            LOG_ERROR("VulkanRenderDevice: No Supported Physical Devices");
        return false;
    }

    // Iterate over all physical devices.
    for (size_t i = 0; i < physical_devices.size(); i++)
    {
        VulkanPhysicalDevicePtr newPhysicalDevice = VulkanPhysicalDevicePtr(new VulkanPhysicalDevice());
        newPhysicalDevice->Initialize(physical_devices[i]);
        m_physicalDevices.push_back(newPhysicalDevice);
    }

    return true; // Initialization successful.
}

void VulkanRenderDevice::VRelease()
{
    FN("VulkanRenderDevice::VRelease");

    if (g_DeviceCount == 0)
    {
        if (m_logicalDevice != nullptr)
        {
            m_logicalDevice.reset();
            m_logicalDevice = nullptr;
        }

        if (g_pVulkanInstance != nullptr)
        {
            g_pVulkanInstance.reset();
            g_pVulkanInstance = nullptr;
        }
        LOG_TRACE("VulkanRenderDevice released");
    }
}

GraphicsWindowPtr VulkanRenderDevice::VCreateWindow(int width, int height, const std::string &title, WindowType type)
{
    FN("VulkanRenderDevice::VCreateWindow");
    OPTICK_EVENT();

    VulkanGraphicsWindowPtr pGraphicsWindow = VulkanGraphicsWindowPtr(new VulkanGraphicsWindow());
    if (pGraphicsWindow->Initialize(this, width, height, title, type))
    {
        if (m_logicalDevice == nullptr)
        {
            if (!pGraphicsWindow->InitializeSurface())
            {
                LOG_ERROR("VulkanRenderDevice: Error while initializing surface!");
                pGraphicsWindow.reset();
                return GraphicsWindowPtr(nullptr);
            }

            m_selectedPhysicalDevice = SelectPhysicalDevice(pGraphicsWindow);
            if (m_selectedPhysicalDevice == nullptr)
            {
                LOG_ERROR("VulkanRenderDevice: Error while selecting physical device!");
                pGraphicsWindow.reset();
                return GraphicsWindowPtr(nullptr);
            }

            m_logicalDevice = m_selectedPhysicalDevice->CreateLogicalDevice(m_graphicsQueueFamilyIdx, m_presentQueueFamilyIdx, m_computeQueueFamilyIndex, m_transferQueueFamilyIndex);
            if (m_logicalDevice == nullptr)
            {
                LOG_ERROR("VulkanRenderDevice: Error while initializing logical device!");
                pGraphicsWindow.reset();
                return GraphicsWindowPtr(nullptr);
            }

            if (!pGraphicsWindow->InitializeSwapchain())
            {
                LOG_ERROR("VulkanRenderDevice: Error while initializing swapchain!");
                pGraphicsWindow.reset();
                return GraphicsWindowPtr(nullptr);
            }
        }

        return pGraphicsWindow;
    }
    else
    {
        LOG_ERROR("VulkanRenderDevice: Error while creating Vulkan-Window!");
        return GraphicsWindowPtr(nullptr);
    }
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename)
{
    FN("VulkanRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, std::string(), FragementShaderFilename);
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename)
{
    FN("VulkanRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, GeometryShaderFilename, FragementShaderFilename, std::string(), std::string());
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename)
{
    FN("VulkanRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    return VCreateShaderProgramFromFile(VertexShaderFilename, FragmentShaderFilename, std::string(), TessControlShaderFilename, TessEvalShaderFilename);
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                                  const std::string &TessEvalShaderFilename)
{
    FN("VulkanRenderDevice::VCreateShaderProgramFromFile");
    OPTICK_EVENT();

    // load file and create shader program
    LOG_ASSERT(VertexShaderFilename.empty(), "VulkanRenderDevice: VertexShaderFilename is empty!");
    LOG_ASSERT(FragmentShaderFilename.empty(), "VulkanRenderDevice: FragmentShaderFilename is empty!");

    std::string vertexShaderSource = "";
    std::string fragmentShaderSource = "";
    std::string geometryShaderSource = "";
    std::string tessControlShaderSource = "";
    std::string tessEvalShaderSource = "";

    LOG_FATAL("VulkanRenderDevice: VCreateShaderProgramFromFile not implemented!");

    // load shader sources from files
    // if (!VertexShaderFilename.empty())
    //{
    //	vertexShaderSource = LoadFile(VertexShaderFilename);
    //}
    // if (!FragmentShaderFilename.empty())
    //{
    //	fragmentShaderSource = LoadFile(FragmentShaderFilename);
    //}
    // if (!GeometryShaderFilename.empty())
    //{
    //	geometryShaderSource = LoadFile(GeometryShaderFilename);
    //}
    // if (!TessControlShaderFilename.empty())
    //{
    //	tessControlShaderSource = LoadFile(TessControlShaderFilename);
    //}
    // if (!TessEvalShaderFilename.empty())
    //{
    //	tessEvalShaderSource = LoadFile(TessEvalShaderFilename);
    //}

    return VCreateShaderProgram(vertexShaderSource, fragmentShaderSource, geometryShaderSource, tessControlShaderSource, tessEvalShaderSource);
}

ComputeShaderProgramPtr VulkanRenderDevice::VCreateComputeShaderProgram(const std::string &computeShaderSource)
{
    FN("VulkanRenderDevice::VCreateComputeShaderProgram");
    OPTICK_EVENT();

    return m_logicalDevice->CreateComputeShaderProgram(computeShaderSource);
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource)
{
    FN("VulkanRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, std::string());
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource)
{
    FN("VulkanRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, GeometryShaderSource, std::string(), std::string());
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource)
{
    FN("VulkanRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return VCreateShaderProgram(VertexShaderSource, FragmentShaderSource, std::string(), TessControlShaderSource, TessEvalShaderSource);
}

ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                                          const std::string &TessEvalShaderSource)
{
    FN("VulkanRenderDevice::VCreateShaderProgram");
    OPTICK_EVENT();

    return m_logicalDevice->CreateShaderProgram(VertexShaderSource, FragmentShaderSource, GeometryShaderSource, TessControlShaderSource, TessEvalShaderSource);
}

MeshBufferPtr VulkanRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
{
    FN("VulkanRenderDevice::VCreateMeshBuffers");
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

VertexBufferPtr VulkanRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes, bool useForRayTracing)
{
    FN("VulkanRenderDevice::VCreateVertexBuffer");
    OPTICK_EVENT();

    return m_logicalDevice->CreateVertexBuffer(usageHint, sizeInBytes, useForRayTracing);
}

IndexBufferPtr VulkanRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes, bool useForRayTracing)
{
    FN("VulkanRenderDevice::VCreateIndexBuffer");
    OPTICK_EVENT();

    return m_logicalDevice->CreateIndexBuffer(usageHint, dataType, sizeInBytes, useForRayTracing);
}

WritePixelBufferPtr VulkanRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
{
    FN("VulkanRenderDevice::VCreateWritePixelBuffer");
    OPTICK_EVENT();

    LOG_FATAL("VulkanRenderDevice: VCreateWritePixelBuffer not implemented!");
    return WritePixelBufferPtr();
}

UniformBufferPtr VulkanRenderDevice::VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("VulkanRenderDevice::VCreateUniformBuffer");
    OPTICK_EVENT();

    VulkanUniformBufferPtr uniformBuffer = m_logicalDevice->CreateUniformBuffer(usageHint, sizeInBytes);
    if (data != nullptr)
    {
        uniformBuffer->VCopyFromSystemMemory(data, 0, sizeInBytes);
    }
    return uniformBuffer;
}

StorageBufferPtr VulkanRenderDevice::VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data)
{
    FN("VulkanRenderDevice::VCreateStorageBuffer");
    OPTICK_EVENT();

    VulkanStorageBufferPtr storageBuffer = m_logicalDevice->CreateStorageBuffer(usageHint, sizeInBytes);
    if (data != nullptr)
    {
        storageBuffer->VCopyFromSystemMemory(data, 0, sizeInBytes);
    }
    return storageBuffer;
}

Texture2DPtr VulkanRenderDevice::VCreateTexture2D(Texture2DDescription description)
{
    FN("VulkanRenderDevice::VCreateTexture2D");
    OPTICK_EVENT();

    VulkanTexture2DPtr texture = VulkanTexture2DPtr(new VulkanTexture2D(m_logicalDevice.get(), description));
    texture->Initialize();
    return texture;
}

Texture2DPtr VulkanRenderDevice::VCreateTexture2D(ImagePtr image)
{
    FN("VulkanRenderDevice::VCreateTexture2D");
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

    VulkanTexture2DPtr texture = VulkanTexture2DPtr(new VulkanTexture2D(m_logicalDevice.get(), Texture2DDescription(image->GetWidth(), image->GetHeight(), format, true)));
    texture->Initialize();

    ImageFormat vulkanFormat = VulkanTypeConverter::TextureToImageFormat(format);
    texture->VCopyFromSystemMemory(image->GetData(), image->GetWidth(), image->GetHeight(), vulkanFormat, datatype);
    return texture;
}

TextureSamplerPtr VulkanRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
{
    FN("VulkanRenderDevice::VCreateTexture2DSampler");
    OPTICK_EVENT();

    VulkanTextureSamplerPtr vulkanSampler = VulkanTextureSamplerPtr(new VulkanTextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy));
    vulkanSampler->Initialize(m_logicalDevice.get());
    return vulkanSampler;
}

std::unique_ptr<IRayTracingShaderProgram> VulkanRenderDevice::VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                                             const std::string &intersectionShaderSource, const std::string &callableShaderSource)
{
    FN("VulkanRenderDevice::VCreateRayTracingShaderProgram");
    OPTICK_EVENT();

    return m_logicalDevice->CreateRayTracingShaderProgram(rayGenShaderSource, anyHitShaderSource, closestHitShaderSource, missShaderSource, intersectionShaderSource, callableShaderSource);
}

std::unique_ptr<IBottomLevelAccelerationStructure> VulkanRenderDevice::VCreateBottomLevelAccelerationStructure(MeshPtr mesh)
{
    FN("VulkanRenderDevice::VCreateBottomLevelAccelerationStructure");

    std::vector<Vector3<float>> positions = mesh->GetVertices();
    std::vector<uint32_t> indices = mesh->GetIndices();

    VulkanBufferPtr vulkanPositionsBuffer =
        m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(Vector3<float>) * mesh->GetNumVertices());
    {
        VulkanBufferPtr stagingBuffer = m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sizeof(Vector3<float>) * mesh->GetNumVertices(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void *mapped = nullptr;
        vkMapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle(), 0, sizeof(Vector3<float>) * mesh->GetNumVertices(), 0, &mapped);
        memcpy(mapped, positions.data(), sizeof(Vector3<float>) * mesh->GetNumVertices());
        vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle());

        VulkanSingleUseCommandBufferPtr copyCmd = m_logicalDevice->CreateSingleUseGraphicsCommandBuffer();
        copyCmd->AllocateAndBegin(m_logicalDevice.get(), m_logicalDevice->GetGraphicsCommandPool().get(), m_logicalDevice->GetGraphicsQueue());
        VkBufferCopy copyRegion = {};
        copyRegion.size = sizeof(Vector3<float>) * mesh->GetNumVertices();
        vkCmdCopyBuffer(copyCmd->GetHandle(), stagingBuffer->GetHandle(), vulkanPositionsBuffer->GetHandle(), 1, &copyRegion);
        copyCmd->EndAndSubmit();
    }

    VulkanBufferPtr vulkanIndexBuffer =
        m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(uint32_t) * mesh->GetNumIndices());
    {
        VulkanBufferPtr stagingBuffer = m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sizeof(uint32_t) * mesh->GetNumIndices(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void *mapped = nullptr;
        vkMapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle(), 0, sizeof(uint32_t) * mesh->GetNumIndices(), 0, &mapped);
        memcpy(mapped, indices.data(), sizeof(uint32_t) * mesh->GetNumIndices());
        vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle());

        VulkanSingleUseCommandBufferPtr copyCmd = m_logicalDevice->CreateSingleUseGraphicsCommandBuffer();
        copyCmd->AllocateAndBegin(m_logicalDevice.get(), m_logicalDevice->GetGraphicsCommandPool().get(), m_logicalDevice->GetGraphicsQueue());
        VkBufferCopy copyRegion = {};
        copyRegion.size = sizeof(uint32_t) * mesh->GetNumIndices();
        vkCmdCopyBuffer(copyCmd->GetHandle(), stagingBuffer->GetHandle(), vulkanIndexBuffer->GetHandle(), 1, &copyRegion);
        copyCmd->EndAndSubmit();
    }

    uint32_t maxPrimitiveCount = mesh->GetNumIndices() / 3;

    VkAccelerationStructureGeometryTrianglesDataKHR trianglesData = {};
    trianglesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    trianglesData.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
    trianglesData.vertexData.deviceAddress = vulkanPositionsBuffer->GetBufferDeviceAddress();
    trianglesData.vertexStride = sizeof(Vector3<float>);
    trianglesData.maxVertex = mesh->GetNumVertices() - 1;
    trianglesData.indexType = VK_INDEX_TYPE_UINT32;
    trianglesData.indexData.deviceAddress = vulkanIndexBuffer->GetBufferDeviceAddress();

    VkAccelerationStructureGeometryKHR geometry = {};
    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geometry.geometry.triangles = trianglesData;

    VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo = {};
    buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_DATA_ACCESS_KHR;
    buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildGeometryInfo.geometryCount = 1;
    buildGeometryInfo.pGeometries = &geometry;

    VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo = {};
    buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(m_logicalDevice->GetHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildGeometryInfo, &maxPrimitiveCount, &buildSizesInfo);

    LOG_TRACE("BLAS Size: %llu, Scratch Size: %llu", buildSizesInfo.accelerationStructureSize, buildSizesInfo.buildScratchSize);

    VulkanBufferPtr blasBuffer = m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, buildSizesInfo.accelerationStructureSize);

    VkAccelerationStructureCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.buffer = blasBuffer->GetHandle();
    createInfo.size = buildSizesInfo.accelerationStructureSize;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

    VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
    VkResult result = vkCreateAccelerationStructureKHR(m_logicalDevice->GetHandle(), &createInfo, nullptr, &accelerationStructure);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create BLAS: %s", VulkanTypeConverter::ToString(result).c_str());
        return nullptr;
    }

    VulkanBufferPtr scratchBuffer =
        m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, buildSizesInfo.buildScratchSize);

    buildGeometryInfo.dstAccelerationStructure = accelerationStructure;
    buildGeometryInfo.scratchData.deviceAddress = scratchBuffer->GetBufferDeviceAddress();

    VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
    buildRangeInfo.primitiveCount = maxPrimitiveCount;
    buildRangeInfo.primitiveOffset = 0;
    buildRangeInfo.firstVertex = 0;
    buildRangeInfo.transformOffset = 0;

    const VkAccelerationStructureBuildRangeInfoKHR *pBuildRangeInfo = &buildRangeInfo;

    VulkanSingleUseCommandBufferPtr cmdBuffer = m_logicalDevice->CreateSingleUseGraphicsCommandBuffer();
    cmdBuffer->AllocateAndBegin(m_logicalDevice.get(), m_logicalDevice->GetGraphicsCommandPool().get(), m_logicalDevice->GetGraphicsQueue());
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer->GetHandle(), 1, &buildGeometryInfo, &pBuildRangeInfo);
    cmdBuffer->EndAndSubmit();

    auto blas = std::make_unique<VulkanBottomLevelAccelerationStructure>(m_logicalDevice.get());
    blas->Initialize(accelerationStructure, blasBuffer, vulkanPositionsBuffer, vulkanIndexBuffer);
    return blas;
}

std::unique_ptr<ITopLevelAccelerationStructure> VulkanRenderDevice::VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures)
{
    FN("VulkanRenderDevice::VCreateTopLevelAccelerationStructure");
    OPTICK_EVENT();

    std::vector<VkAccelerationStructureInstanceKHR> instances(bottomLevelAccelerationStructures.size());
    for (size_t i = 0; i < bottomLevelAccelerationStructures.size(); i++)
    {
        auto *vulkanBlas = static_cast<VulkanBottomLevelAccelerationStructure *>(bottomLevelAccelerationStructures[i].get());

        VkAccelerationStructureInstanceKHR &instance = instances[i];
        memset(&instance, 0, sizeof(instance));
        // Identity transform
        instance.transform.matrix[0][0] = 1.0f;
        instance.transform.matrix[1][1] = 1.0f;
        instance.transform.matrix[2][2] = 1.0f;
        instance.instanceCustomIndex = 0;
        instance.mask = 0xFF;
        instance.instanceShaderBindingTableRecordOffset = 0;
        instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        instance.accelerationStructureReference = vulkanBlas->GetDeviceAddress();
    }

    VkDeviceSize instancesSize = sizeof(VkAccelerationStructureInstanceKHR) * instances.size();
    VulkanBufferPtr instancesBuffer = m_logicalDevice->CreateBuffer(
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, instancesSize);
    {
        VulkanBufferPtr stagingBuffer = m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, instancesSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void *mapped = nullptr;
        vkMapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle(), 0, instancesSize, 0, &mapped);
        memcpy(mapped, instances.data(), instancesSize);
        vkUnmapMemory(m_logicalDevice->GetHandle(), stagingBuffer->GetDeviceMemory()->GetHandle());

        VulkanSingleUseCommandBufferPtr copyCmd = m_logicalDevice->CreateSingleUseGraphicsCommandBuffer();
        copyCmd->AllocateAndBegin(m_logicalDevice.get(), m_logicalDevice->GetGraphicsCommandPool().get(), m_logicalDevice->GetGraphicsQueue());
        VkBufferCopy copyRegion = {};
        copyRegion.size = instancesSize;
        vkCmdCopyBuffer(copyCmd->GetHandle(), stagingBuffer->GetHandle(), instancesBuffer->GetHandle(), 1, &copyRegion);
        copyCmd->EndAndSubmit();
    }

    VkAccelerationStructureGeometryInstancesDataKHR instancesData = {};
    instancesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    instancesData.arrayOfPointers = VK_FALSE;
    instancesData.data.deviceAddress = instancesBuffer->GetBufferDeviceAddress();

    VkAccelerationStructureGeometryKHR geometry = {};
    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
    geometry.geometry.instances = instancesData;

    VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo = {};
    buildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    buildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildGeometryInfo.geometryCount = 1;
    buildGeometryInfo.pGeometries = &geometry;

    uint32_t instanceCount = static_cast<uint32_t>(instances.size());
    VkAccelerationStructureBuildSizesInfoKHR buildSizesInfo = {};
    buildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
    vkGetAccelerationStructureBuildSizesKHR(m_logicalDevice->GetHandle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildGeometryInfo, &instanceCount, &buildSizesInfo);

    LOG_TRACE("TLAS Size: %llu, Scratch Size: %llu", buildSizesInfo.accelerationStructureSize, buildSizesInfo.buildScratchSize);

    VulkanBufferPtr tlasBuffer = m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR, buildSizesInfo.accelerationStructureSize);

    VkAccelerationStructureCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.buffer = tlasBuffer->GetHandle();
    createInfo.size = buildSizesInfo.accelerationStructureSize;
    createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

    VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
    VkResult result = vkCreateAccelerationStructureKHR(m_logicalDevice->GetHandle(), &createInfo, nullptr, &accelerationStructure);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create TLAS: %s", VulkanTypeConverter::ToString(result).c_str());
        return nullptr;
    }

    VulkanBufferPtr scratchBuffer =
        m_logicalDevice->CreateBuffer(VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, buildSizesInfo.buildScratchSize);

    buildGeometryInfo.dstAccelerationStructure = accelerationStructure;
    buildGeometryInfo.scratchData.deviceAddress = scratchBuffer->GetBufferDeviceAddress();

    VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
    buildRangeInfo.primitiveCount = instanceCount;
    const VkAccelerationStructureBuildRangeInfoKHR *pBuildRangeInfo = &buildRangeInfo;

    VulkanSingleUseCommandBufferPtr cmdBuffer = m_logicalDevice->CreateSingleUseGraphicsCommandBuffer();
    cmdBuffer->AllocateAndBegin(m_logicalDevice.get(), m_logicalDevice->GetGraphicsCommandPool().get(), m_logicalDevice->GetGraphicsQueue());
    vkCmdBuildAccelerationStructuresKHR(cmdBuffer->GetHandle(), 1, &buildGeometryInfo, &pBuildRangeInfo);
    cmdBuffer->EndAndSubmit();

    auto tlas = std::make_unique<VulkanTopLevelAccelerationStructure>(m_logicalDevice.get());
    tlas->Initialize(accelerationStructure, tlasBuffer, instancesBuffer);
    return tlas;
}

VulkanInstancePtr VulkanRenderDevice::GetInstance()
{
    FN("VulkanRenderDevice::GetInstance");

    return g_pVulkanInstance;
}

std::vector<VulkanPhysicalDevicePtr> VulkanRenderDevice::GetAvailablePhysicalDevices()
{
    FN("VulkanRenderDevice::GetAvailablePhysicalDevices");

    return m_physicalDevices;
}

VulkanPhysicalDevicePtr VulkanRenderDevice::GetSelectedPhysicalDevice()
{
    FN("VulkanRenderDevice::GetSelectedPhysicalDevice");

    return m_selectedPhysicalDevice;
}

VulkanLogicalDevicePtr VulkanRenderDevice::GetLogicalDevice()
{
    FN("VulkanRenderDevice::GetLogicalDevice");

    return m_logicalDevice;
}

// =================================================================================================
// PRIVATE METHODS
// =================================================================================================

VulkanPhysicalDevicePtr VulkanRenderDevice::SelectPhysicalDevice(VulkanGraphicsWindowPtr graphicsWindow)
{
    FN("VulkanRenderDevice::SelectPhysicalDevice");

    std::vector<VulkanPhysicalDevicePtr> physicalDevices = GetAvailablePhysicalDevices();
    for (int i = 0; i < physicalDevices.size(); i++)
    {
        VulkanRenderSurfacePtr surface = std::static_pointer_cast<VulkanRenderSurface>(graphicsWindow->VGetContext());
        physicalDevices[i]->CheckSurfaceCapabilities(surface->GetHandle());

        int graphicsIdx = -1;
        int presentIdx = -1;
        int computeIdx = -1;
        int transferIdx = -1;

        if (physicalDevices[i]->GetSurfaceFormats().size() == 0)
            continue;

        if (physicalDevices[i]->GetPresentModes().size() == 0)
            continue;

        if (physicalDevices[i]->GetPhysicalDeviceProperties().properties.apiVersion < VK_API_VERSION_1_2)
            continue;

        // Find graphics queue family
        for (int j = 0; j < physicalDevices[i]->GetQueueFamilies().size(); ++j)
        {
            VulkanQueueFamilyPtr queueFamily = physicalDevices[i]->GetQueueFamilies()[j];

            if (queueFamily->GetQueueCount() == 0)
            {
                continue;
            }

            if (queueFamily->IsGraphicsQueue())
            {
                // Got it!
                graphicsIdx = queueFamily->GetFamilyIndex();
                break;
            }
        }

        // Find present queue family
        for (int j = 0; j < physicalDevices[i]->GetQueueFamilies().size(); ++j)
        {
            VulkanQueueFamilyPtr queueFamily = physicalDevices[i]->GetQueueFamilies()[j];

            if (queueFamily->GetQueueCount() == 0)
            {
                continue;
            }

            if (queueFamily->IsPresentQueue(physicalDevices[i]->GetHandle(), queueFamily->GetFamilyIndex(), surface->GetHandle()))
            {
                // Got it!
                presentIdx = queueFamily->GetFamilyIndex();
                break;
            }
        }

        // Find compute family
        for (int j = 0; j < physicalDevices[i]->GetQueueFamilies().size(); ++j)
        {
            VulkanQueueFamilyPtr queueFamily = physicalDevices[i]->GetQueueFamilies()[j];

            if (queueFamily->GetQueueCount() == 0)
            {
                continue;
            }

            if (queueFamily->IsComputeQueue())
            {
                // Got it!
                computeIdx = queueFamily->GetFamilyIndex();
                break;
            }
        }

        // Find compute family
        for (int j = 0; j < physicalDevices[i]->GetQueueFamilies().size(); ++j)
        {
            VulkanQueueFamilyPtr queueFamily = physicalDevices[i]->GetQueueFamilies()[j];

            if (queueFamily->GetQueueCount() == 0)
            {
                continue;
            }

            if (queueFamily->IsTransferQueue())
            {
                // Got it!
                transferIdx = queueFamily->GetFamilyIndex();
                break;
            }
        }

        // Did we find a device supporting all required queue families?
        if (graphicsIdx >= 0 && presentIdx >= 0 && computeIdx >= 0 && transferIdx >= 0)
        {
            m_graphicsQueueFamilyIdx = static_cast<uint32_t>(graphicsIdx);
            m_presentQueueFamilyIdx = static_cast<uint32_t>(presentIdx);
            m_computeQueueFamilyIndex = static_cast<uint32_t>(computeIdx);
            m_transferQueueFamilyIndex = static_cast<uint32_t>(transferIdx);
            return physicalDevices[i];
        }

        LOG_ERROR("Could not find a physical device which fits our desired profile!");
        return VulkanPhysicalDevicePtr(nullptr);
    }
}

} // namespace bow