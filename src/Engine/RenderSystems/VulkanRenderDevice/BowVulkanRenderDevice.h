#pragma once
#include "BowPrerequisites.h"

#include "IBowRenderDevice.h"

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLGraphicsWindow> OGLGraphicsWindowPtr;
		typedef std::unordered_map<int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;

		class VulkanRenderDevice : public IRenderDevice
		{
		public:
			VulkanRenderDevice(void);
			~VulkanRenderDevice(void);

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			bool Init(void);
			void VRelease(void);

			GraphicsWindowPtr VCreateWindow(int width, int height, const std::string& title, WindowType type);

			// =========================================================================
			// SHADER STUFF:
			// =========================================================================
			ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename);
			ShaderProgramPtr		VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename);

			ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource);
			ShaderProgramPtr		VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource);

			VertexBufferPtr			VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes);
			IndexBufferPtr			VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes);

			WritePixelBufferPtr		VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

			Texture2DPtr			VCreateTexture2DFromFile(const std::string& Filename, bool generateMipmaps = false);
			Texture2DPtr			VCreateTexture2D(Core::Bitmap* bitmap, TextureFormat format, bool generateMipmaps = false);
			Texture2DPtr			VCreateTexture2D(Texture2DDescription description);

			TextureSamplerPtr		VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy);

		private:
			//you shall not copy
			VulkanRenderDevice(VulkanRenderDevice&) {}
			VulkanRenderDevice& operator=(const VulkanRenderDevice&) { return *this; }

			int	m_maximumNumberOfVertexAttributes;
			int	m_numberOfTextureUnits;
			int	m_maximumNumberOfColorAttachments;

			std::unordered_map<int, Texture2DPtr> textures;
		};

		typedef std::shared_ptr<VulkanRenderDevice> OGLRenderDevicePtr;
		typedef std::unordered_map<int, VulkanRenderDevice> OGLRenderDeviceMap;

	}
}