#include "BowVulkanRenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "IBowTexture2D.h"

namespace Bow {
	namespace Renderer {

		using namespace Core;

		VulkanRenderDevice::VulkanRenderDevice(void)
		{

		}

		VulkanRenderDevice::~VulkanRenderDevice(void)
		{
			VRelease();
		}

		bool VulkanRenderDevice::Init(void)
		{
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

		Texture2DPtr VulkanRenderDevice::VCreateTexture2DFromFile(const std::string& Filename, bool generateMipmaps)
		{
			return Texture2DPtr(nullptr);
		}

		Texture2DPtr VulkanRenderDevice::VCreateTexture2D(Bitmap* bitmap, TextureFormat format, bool generateMipmaps)
		{
			return Texture2DPtr(nullptr);
		}

		Texture2DPtr VulkanRenderDevice::VCreateTexture2D(Texture2DDescription description)
		{
			return Texture2DPtr(nullptr);
		}

		TextureSamplerPtr VulkanRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
		{
			return TextureSamplerPtr(nullptr);
		}

	}
}