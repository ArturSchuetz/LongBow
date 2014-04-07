#include "BowOGLRenderDevice.h"
#include "BowLogger.h"

#include "BowOGLGraphicsWindow.h"
#include "BowOGLShaderProgram.h"

#include "BowOGLIndexBuffer.h"
#include "BowOGLVertexBuffer.h"
#include "BowOGLWritePixelBuffer.h"

#include "BowOGLTexture2D.h"
#include "BowOGLTextureSampler.h"
#include "BowTexture2DDescription.h"

#include "BowBitmap.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {

		using namespace Core;

		OGLRenderDevice::OGLRenderDevice(HINSTANCE hInstance)
		{
			m_hInstance = hInstance;
			m_currentWindow = 0;
		}

		OGLRenderDevice::~OGLRenderDevice(void)
		{
			VRelease();
		}

		GraphicsWindowPtr OGLRenderDevice::VCreateWindow(int width, int height, std::string title, WindowType type)
		{
			// Initialise GLFW
			OGLGraphicsWindowPtr pGraphicsWindow = OGLGraphicsWindowPtr(new OGLGraphicsWindow());

			if (!pGraphicsWindow->VInitialize(m_hInstance, width, height, title))
			{
				return OGLGraphicsWindowPtr(nullptr);
			}
			glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &m_maximumNumberOfVertexAttributes);
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_numberOfTextureUnits);
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &m_maximumNumberOfColorAttachments);

			m_GraphicsWindowMap.insert(std::pair<int, OGLGraphicsWindowPtr>(m_currentWindow++, pGraphicsWindow));
			return pGraphicsWindow;
		}

		void OGLRenderDevice::VRelease(void)
		{
			for (auto it = m_GraphicsWindowMap.begin(); it != m_GraphicsWindowMap.end(); it++)
			{
				if (it->second != nullptr)
				{
					it->second->VRelease();
				}
			}
			m_GraphicsWindowMap.clear();
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

		IndexBufferPtr OGLRenderDevice::VCreateIndexBuffer(BufferHint usageHint, int sizeInBytes)
		{
			return OGLIndexBufferPtr(new OGLIndexBuffer(usageHint, sizeInBytes));
		}

		WritePixelBufferPtr OGLRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
		{
			return OGLWritePixelBufferPtr(new OGLWritePixelBuffer(usageHint, sizeInBytes));
		}

		Texture2DPtr OGLRenderDevice::VCreateTexture2DFromFile(const std::string& Filename, bool generateMipmaps)
		{
			Bitmap dayBitmap;
			if (!dayBitmap.LoadFile(Filename.c_str()))
			{
				LOG_ERROR("Couldn't find Texture: %s", Filename.c_str());
				return Texture2DPtr(nullptr);
			}

			return VCreateTexture2D(dayBitmap, TextureFormat::RedGreenBlue8, generateMipmaps);
		}

		Texture2DPtr OGLRenderDevice::VCreateTexture2D(Bitmap dayBitmap, TextureFormat format, bool generateMipmaps)
		{
			// Create Description
			Texture2DDescription description(dayBitmap.GetWidth(), dayBitmap.GetHeight(), format, generateMipmaps);

			// Create and Fill Texture
			auto texture = OGLTexture2DPtr(new OGLTexture2D(description, GL_TEXTURE_2D));
			texture->CopyFromSystemMemory(dayBitmap.GetData(), 0, 0, dayBitmap.GetWidth(), dayBitmap.GetHeight(), ImageFormat::BlueGreenRed, ImageDatatype::UnsignedByte, 4);

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