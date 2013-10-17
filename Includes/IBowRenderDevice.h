#pragma once
#include "LongBow.h"
#include "IBowGraphicsWindow.h"
#include "IBowShaderProgram.h"

#include "IBowTexture2D.h"
#include "IBowTextureSampler.h"

#include "BowBitmap.h"

namespace Bow
{
	class IRenderDevice
	{
	public:
		IRenderDevice(void){};
		virtual ~IRenderDevice(void){};
		virtual void VRelease(void) = 0;

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		virtual GraphicsWindowPtr		VCreateWindow( int width = 800, int height = 600, std::string title = "LongBow Game" ) = 0;

		virtual ShaderProgramPtr		VCreateShaderProgramFromFile(	const std::string& VertexShaderFilename,
																		const std::string& FragementShaderFilename ) = 0;
	
		virtual ShaderProgramPtr		VCreateShaderProgramFromFile(	const std::string& VertexShaderFilename,
																		const std::string& GeometryShaderFilename,
																		const std::string& FragementShaderFilename ) = 0;

		virtual ShaderProgramPtr		VCreateShaderProgram(	const std::string& VertexShaderSource,
																const std::string& FragementShaderSource ) = 0;

		virtual ShaderProgramPtr		VCreateShaderProgram(	const std::string& VertexShaderSource,
																const std::string& GeometryShaderSource,
																const std::string& FragementShaderSource ) = 0;

		virtual VertexBufferPtr			VCreateVertexBuffer( BufferHint usageHint, int sizeInBytes ) = 0;

		virtual IndexBufferPtr			VCreateIndexBuffer( BufferHint usageHint, int sizeInBytes ) = 0;

		virtual WritePixelBufferPtr		VCreateWritePixelBuffer( PixelBufferHint usageHint, int sizeInBytes ) = 0;
		
        virtual Texture2DPtr			VCreateTexture2DFromFile( const std::string& Filename, bool generateMipmaps = false ) = 0;

		virtual Texture2DPtr			VCreateTexture2D( Bitmap dayBitmap, TextureFormat format, bool generateMipmaps = false ) = 0;
		virtual Texture2DPtr			VCreateTexture2D( Texture2DDescription description ) = 0;

		virtual TextureSamplerPtr		VCreateTexture2DSampler( TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1) = 0;
	};

	typedef std::shared_ptr<IRenderDevice> RenderDevicePtr;
}