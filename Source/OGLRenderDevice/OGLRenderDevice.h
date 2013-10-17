#pragma once
#include "LongBow.h"
#include "IBowRenderDevice.h"
#include "OGLGraphicsWindow.h"
#include "OGLShaderProgram.h"

namespace Bow {

	class OGLRenderDevice : public IRenderDevice
	{
	public:
		OGLRenderDevice(HINSTANCE hInstance);
		~OGLRenderDevice(void);

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		GraphicsWindowPtr VCreateWindow(int width, int height, std::string title);
		void VRelease(void);

		// =========================================================================
		// SHADER STUFF:
		// =========================================================================
		ShaderProgramPtr		VCreateShaderProgramFromFile( const std::string& VertexShaderFilename, const std::string& FragementShaderFilename);
		ShaderProgramPtr		VCreateShaderProgramFromFile( const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename);

		ShaderProgramPtr		VCreateShaderProgram( const std::string& VertexShaderSource, const std::string& FragementShaderSource);		
		ShaderProgramPtr		VCreateShaderProgram( const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource);

		VertexBufferPtr			VCreateVertexBuffer( BufferHint usageHint, int sizeInBytes );
		IndexBufferPtr			VCreateIndexBuffer( BufferHint usageHint, int sizeInBytes );

		WritePixelBufferPtr		VCreateWritePixelBuffer( PixelBufferHint usageHint, int sizeInBytes );

		Texture2DPtr			VCreateTexture2DFromFile( const std::string& Filename, bool generateMipmaps = false );
		Texture2DPtr			VCreateTexture2D( Bitmap dayBitmap, TextureFormat format, bool generateMipmaps = false );
        Texture2DPtr			VCreateTexture2D( Texture2DDescription description );

		TextureSamplerPtr		VCreateTexture2DSampler( TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy);
	
	private:
		int	m_maximumNumberOfVertexAttributes;
		int	m_numberOfTextureUnits;
		int	m_maximumNumberOfColorAttachments;

		int	m_currentWindow;
		OGLGraphicsWindowMap m_GraphicsWindowMap;

		HINSTANCE m_hInstance;
	};

	typedef std::shared_ptr<OGLRenderDevice> OGLRenderDevicePtr;
	typedef std::hash_map<int, OGLRenderDevice> OGLRenderDeviceMap;

}