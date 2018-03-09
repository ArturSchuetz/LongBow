#include "BowVulkanRenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "IBowTexture2D.h"

#include "vulkan.h"
#include "OperatingSystem.h"
#include "VulkanFunctions.h"

namespace bow {
	
	VulkanRenderDevice::VulkanRenderDevice(void) :
		VulkanLibrary()
	{

	}

	VulkanRenderDevice::~VulkanRenderDevice(void)
	{
		VRelease();
	}

	bool VulkanRenderDevice::Initialize(void)
	{
		LOG_TRACE("Loadnig Vulkan Library frim DLL.");

#if defined(_WIN32) || defined(_WIN64)
		VulkanLibrary = LoadLibrary(L"vulkan-1.dll");
#elif defined(__linux__) || defined(__unix__)
		VulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW);
#else
#error "Unknown compiler"
#endif
		if (VulkanLibrary == nullptr) {
			LOG_ERROR("Could not load Vulkan library! Please make sure you installed Vulkan drivers!");
			return false;
		}

#if defined(_WIN32) || defined(_WIN64)
#define LoadProcAddress GetProcAddress
#elif defined(__linux__) || defined(__unix__)
#define LoadProcAddress dlsym
#endif

//#define VK_EXPORTED_FUNCTION( fun ) if( !(fun = ((PFN_##fun)LoadProcAddress( VulkanLibrary, #fun ))) ) { LOG_ERROR("Could not load exported function: %s", #fun); return false; }

#include "ListOfFunctions.inl"

		return true;
	}

	void VulkanRenderDevice::VRelease(void)
	{
	}

	GraphicsWindowPtr VulkanRenderDevice::VCreateWindow(int width, int height, const std::string& title, WindowType type)
	{
		return GraphicsWindowPtr(nullptr);
	}

	// =========================================================================
	// SHADER STUFF:
	// =========================================================================
	ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr VulkanRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	MeshBufferPtr VulkanRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		return MeshBufferPtr(nullptr);
	}

	VertexBufferPtr	VulkanRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
	{
		return VertexBufferPtr(nullptr);
	}

	IndexBufferPtr VulkanRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
	{
		return IndexBufferPtr(nullptr);
	}

	WritePixelBufferPtr VulkanRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
	{
		return WritePixelBufferPtr(nullptr);
	}

	Texture2DPtr VulkanRenderDevice::VCreateTexture2D(Texture2DDescription description)
	{
		return Texture2DPtr(nullptr);
	}

	Texture2DPtr VulkanRenderDevice::VCreateTexture2D(ImagePtr image, TextureFormat format)
	{
		return Texture2DPtr(nullptr);
	}

	TextureSamplerPtr VulkanRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
	{
		return TextureSamplerPtr(nullptr);
	}

}
