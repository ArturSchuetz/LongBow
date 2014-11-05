#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "IBowGraphicsWindow.h"

namespace Bow {
	namespace Renderer{

		using namespace Core;

		enum class WindowType : char
		{
			Windowed = 0,
			Fullscreen
		};

		/**
		* \~german
		* \todo Tessalation implementieren
		**/
		class IRenderDevice
		{
		public:
			IRenderDevice(void){};
			virtual ~IRenderDevice(void){}
			virtual void VRelease(void) = 0;

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			virtual GraphicsWindowPtr	VCreateWindow(int width = 800, int height = 600, const std::string& title = "LongBow Game", WindowType type = WindowType::Windowed) = 0;

			virtual ShaderProgramPtr	VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename) = 0;
			virtual ShaderProgramPtr	VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename) = 0;

			virtual ShaderProgramPtr	VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource) = 0;
			virtual ShaderProgramPtr	VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource) = 0;

			virtual VertexBufferPtr		VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes) = 0;
			virtual IndexBufferPtr		VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes) = 0;
			virtual WritePixelBufferPtr	VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) = 0;

			virtual Texture2DPtr		VCreateTexture2DFromFile(const std::string& Filename, bool generateMipmaps = false) = 0;
			virtual Texture2DPtr		VCreateTexture2D(Bitmap bitmap, TextureFormat format, bool generateMipmaps = false) = 0;
			virtual Texture2DPtr		VCreateTexture2D(Texture2DDescription description) = 0;
			virtual TextureSamplerPtr	VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1) = 0;
		};

	}
}