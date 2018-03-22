#pragma once
#include "BowPrerequisites.h"

#include "IBowRenderDevice.h"

#include "vulkan.h"
#include "OperatingSystem.h"

namespace bow {

	typedef std::shared_ptr<class OGLGraphicsWindow> OGLGraphicsWindowPtr;
	typedef std::unordered_map<unsigned int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;

	class VulkanRenderDevice : public IRenderDevice
	{
	public:
		VulkanRenderDevice(void);
		~VulkanRenderDevice(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		bool Initialize(void);
		void VRelease(void);

		GraphicsWindowPtr		VCreateWindow(int width, int height, const std::string& title, WindowType type);

		// =========================================================================
		// SHADER STUFF:
		// =========================================================================
		ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename);
		ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename);

		ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource);
		ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource);

		MeshBufferPtr			VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint);
		VertexBufferPtr			VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes);
		IndexBufferPtr			VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes);

		WritePixelBufferPtr		VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

		Texture2DPtr			VCreateTexture2D(Texture2DDescription description);
		Texture2DPtr			VCreateTexture2D(ImagePtr image);

		TextureSamplerPtr		VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy);

	private:
		//you shall not copy
		VulkanRenderDevice(VulkanRenderDevice&) {}
		VulkanRenderDevice& operator=(const VulkanRenderDevice&) { return *this; }

		LibraryHandle VulkanLibrary;
	};

	typedef std::shared_ptr<VulkanRenderDevice> OGLRenderDevicePtr;
	typedef std::unordered_map<unsigned int, VulkanRenderDevice> OGLRenderDeviceMap;

}
