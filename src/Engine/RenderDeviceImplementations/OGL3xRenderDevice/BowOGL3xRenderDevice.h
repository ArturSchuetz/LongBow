#pragma once
#include "BowPrerequisites.h"

#include "IBowRenderDevice.h"

namespace bow {

	typedef std::shared_ptr<class OGLGraphicsWindow> OGLGraphicsWindowPtr;
	typedef std::unordered_map<unsigned int, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;

	class OGLRenderDevice : public IRenderDevice
	{
	public:
		OGLRenderDevice(void);
		~OGLRenderDevice(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		bool Initialize(void);
		void VRelease(void);

		GraphicsWindowPtr VCreateWindow(int width, int height, const std::string& title, WindowType type);

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
		OGLRenderDevice(OGLRenderDevice&){}
		OGLRenderDevice& operator=(const OGLRenderDevice&) { return *this; }

		int	m_maximumNumberOfVertexAttributes;
		int	m_numberOfTextureUnits;
		int	m_maximumNumberOfColorAttachments;

		std::unordered_map<unsigned int, Texture2DPtr> textures;
	};

	typedef std::shared_ptr<OGLRenderDevice> OGLRenderDevicePtr;
	typedef std::unordered_map<unsigned int, OGLRenderDevice> OGLRenderDeviceMap;

}
