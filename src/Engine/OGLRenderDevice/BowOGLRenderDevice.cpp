#include "BowOGLRenderDevice.h"
#include "BowLogger.h"

#include "BowOGLGraphicsWindow.h"
#include "BowOGLShaderProgram.h"

#include "IBowIndicesBase.h"
#include "BowOGLIndexBuffer.h"

#include "BowOGLVertexBuffer.h"
#include "BowOGLWritePixelBuffer.h"

#include "BowOGLTexture2D.h"
#include "BowOGLTextureSampler.h"

#include "BowBitmap.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace Bow {
	namespace Renderer {

		using namespace Core;

		void error_callback(int error, const char* description)
		{
			LOG_ERROR(description);
		}


		OGLRenderDevice::OGLRenderDevice(void)
		{
			glfwSetErrorCallback(error_callback);
			if (GL_TRUE != glfwInit())
			{
				LOG_ERROR("Could not initialize GLFW!");
			}
			else
			{
				LOG_TRACE("GLFW sucessfully initialized!");
			}
		}


		OGLRenderDevice::~OGLRenderDevice(void)
		{
			VRelease();
		}


		void OGLRenderDevice::VRelease(void)
		{
			glfwTerminate();
			LOG_TRACE("RenderDeviceOGL3x released.");
		}


		GraphicsWindowPtr OGLRenderDevice::VCreateWindow(int width, int height, const std::string& title, WindowType type)
		{
			OGLGraphicsWindowPtr pGraphicsWindow = OGLGraphicsWindowPtr(new OGLGraphicsWindow());
			if (pGraphicsWindow->Initialize(width, height, title, type))
			{
				glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &m_maximumNumberOfVertexAttributes);
				LOG_TRACE("\tMaximum number of Vertex Attributes: %i", m_maximumNumberOfVertexAttributes);

				glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_numberOfTextureUnits);
				LOG_TRACE("\tMaximum number of Texture Units: %i", m_numberOfTextureUnits);

				glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maximumNumberOfColorAttachments);
				LOG_TRACE("\tMaximum number of Color Attachments: %i", m_maximumNumberOfColorAttachments);

				return pGraphicsWindow;
			}
			else
			{
				LOG_ERROR("Error while creating OpenGL-Window!");
				return GraphicsWindowPtr(nullptr);
			}
		}

		// =========================================================================
		// SHADER STUFF:
		// =========================================================================
		ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename)
		{
			//Open file
			std::string vshaderString;
			std::ifstream vsourceFile(VertexShaderFilename.c_str());

			//Source file loaded
			if (vsourceFile)
			{
				//Get shader source
				vshaderString.assign((std::istreambuf_iterator< char >(vsourceFile)), std::istreambuf_iterator< char >());

				//Open file
				std::string fshaderString;
				std::ifstream fsourceFile(FragementShaderFilename.c_str());

				if (fsourceFile)
				{
					//Get shader source
					fshaderString.assign((std::istreambuf_iterator< char >(fsourceFile)), std::istreambuf_iterator< char >());
					return VCreateShaderProgram(vshaderString, fshaderString);
				}
				else
				{
					LOG_ERROR("Could not open Shader from File");
					return ShaderProgramPtr();
				}
			}
			else
			{
				LOG_ERROR("Could not open Shader from File");
				return ShaderProgramPtr();
			}
		}


		ShaderProgramPtr OGLRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename)
		{
			//Open file
			std::string vshaderString;
			std::ifstream vsourceFile(VertexShaderFilename.c_str());

			//Source file loaded
			if (vsourceFile)
			{
				//Get shader source
				vshaderString.assign((std::istreambuf_iterator< char >(vsourceFile)), std::istreambuf_iterator< char >());

				//Open file
				std::string gshaderString;
				std::ifstream gsourceFile(GeometryShaderFilename.c_str());

				if (gsourceFile)
				{
					//Get shader source
					gshaderString.assign((std::istreambuf_iterator< char >(gsourceFile)), std::istreambuf_iterator< char >());

					//Open file
					std::string fshaderString;
					std::ifstream fsourceFile(FragementShaderFilename.c_str());

					if (fsourceFile)
					{
						//Get shader source
						fshaderString.assign((std::istreambuf_iterator< char >(fsourceFile)), std::istreambuf_iterator< char >());
						return VCreateShaderProgram(vshaderString, gshaderString, fshaderString);
					}
					else
					{
						LOG_ERROR("Could not open Shader from File");
						return OGLShaderProgramPtr(nullptr);
					}
				}
				else
				{
					LOG_ERROR("Could not open Shader from File");
					return OGLShaderProgramPtr(nullptr);
				}
			}
			else
			{
				LOG_ERROR("Could not open Shader from File");
				return OGLShaderProgramPtr(nullptr);
			}
		}

		ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource)
		{
			return VCreateShaderProgram(VertexShaderSource, std::string(), FragementShaderSource);
		}

		ShaderProgramPtr OGLRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource)
		{
			return OGLShaderProgramPtr(new OGLShaderProgram(VertexShaderSource, GeometryShaderSource, FragementShaderSource));
		}

		VertexBufferPtr	OGLRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
		{
			return OGLVertexBufferPtr(new OGLVertexBuffer(usageHint, sizeInBytes));
		}

		IndexBufferPtr OGLRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
		{
			return OGLIndexBufferPtr(new OGLIndexBuffer(usageHint, dataType, sizeInBytes));
		}

		WritePixelBufferPtr OGLRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
		{
			return OGLWritePixelBufferPtr(new OGLWritePixelBuffer(usageHint, sizeInBytes));
		}

		Texture2DPtr OGLRenderDevice::VCreateTexture2DFromFile(const std::string& Filename, bool generateMipmaps)
		{
			Bitmap bitmap;
			if (!bitmap.LoadFile(Filename.c_str()))
			{
				LOG_ERROR("Couldn't find Texture: %s", Filename.c_str());
				return Texture2DPtr(nullptr);
			}

			return VCreateTexture2D(bitmap, TextureFormat::RedGreenBlue8, generateMipmaps);
		}

		Texture2DPtr OGLRenderDevice::VCreateTexture2D(Bitmap bitmap, TextureFormat format, bool generateMipmaps)
		{
			// Create Description
			Texture2DDescription description(bitmap.GetWidth(), bitmap.GetHeight(), format, generateMipmaps);

			// Create and Fill Texture
			auto texture = OGLTexture2DPtr(new OGLTexture2D(description, GL_TEXTURE_2D));
			texture->VCopyFromSystemMemory(bitmap.GetData(), 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), ImageFormat::BlueGreenRed, ImageDatatype::UnsignedByte, 4);

			return texture;
		}

		Texture2DPtr OGLRenderDevice::VCreateTexture2D(Texture2DDescription description)
		{
			return OGLTexture2DPtr(new OGLTexture2D(description, GL_TEXTURE_2D));
		}

		TextureSamplerPtr OGLRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
		{
			return OGLTextureSamplerPtr(new OGLTextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy));
		}

	}
}