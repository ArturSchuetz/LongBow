#include "BowOGL3xRenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "BowOGL3xGraphicsWindow.h"
#include "BowOGL3xShaderProgram.h"
#include "BowShaderVertexAttribute.h"
#include "BowVertexBufferAttribute.h"
#include "IBowVertexAttribute.h"

#include "IBowIndicesBase.h"
#include "BowOGL3xIndexBuffer.h"

#include "BowOGL3xVertexBuffer.h"
#include "BowOGL3xWritePixelBuffer.h"

#include "BowMesh.h"
#include "BowMeshBuffers.h"

#include "BowOGL3xTexture2D.h"
#include "BowOGL3xTextureSampler.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace bow {
	
	void error_callback(int error, const char* description)
	{
		LOG_ERROR(description);
	}

	OGLRenderDevice::OGLRenderDevice(void)
	{
			
	}

	OGLRenderDevice::~OGLRenderDevice(void)
	{
		VRelease();
	}

	bool OGLRenderDevice::Initialize(void)
	{
		glfwSetErrorCallback(error_callback);
		if (GL_TRUE != glfwInit())
		{
			LOG_ERROR("Could not initialize GLFW!");
			return false;
		}
		else
		{
			LOG_TRACE("GLFW sucessfully initialized!");
		}
		return true;
	}

	void OGLRenderDevice::VRelease(void)
	{
		glfwTerminate();
		LOG_TRACE("RenderDeviceOGL3x released.");
	}

	GraphicsWindowPtr OGLRenderDevice::VCreateWindow(int width, int height, const std::string& title, WindowType type)
	{
		OGLGraphicsWindowPtr pGraphicsWindow = OGLGraphicsWindowPtr(new OGLGraphicsWindow());
		if (pGraphicsWindow->Initialize(width, height, title, type, this))
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

	MeshBufferPtr OGLRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		MeshBuffers *meshBuffers = new MeshBuffers();

		if (mesh.Indices != nullptr)
		{
			if (mesh.Indices->Type == IndicesType::UnsignedShort)
			{
				std::vector<unsigned short> meshIndices = (std::dynamic_pointer_cast<IndicesUnsignedShort>(mesh.Indices))->Values;

				std::vector<unsigned short> indices = std::vector<unsigned short>(meshIndices.size());
				for (unsigned int j = 0; j < meshIndices.size(); ++j)
				{
					indices[j] = meshIndices[j];
				}

				IndexBufferPtr indexBuffer = VCreateIndexBuffer(usageHint, IndexBufferDatatype::UnsignedShort, indices.size() * sizeof(unsigned short));
				indexBuffer->VCopyFromSystemMemory(&(indices[0]), indices.size() * sizeof(unsigned short));
				meshBuffers->IndexBuffer = indexBuffer;
			}
			else if (mesh.Indices->Type == IndicesType::UnsignedInt)
			{
				std::vector<unsigned int> meshIndices = (std::dynamic_pointer_cast<IndicesUnsignedInt>(mesh.Indices))->Values;

				std::vector<unsigned int> indices = std::vector<unsigned int>(meshIndices.size());
				for (unsigned int j = 0; j < meshIndices.size(); ++j)
				{
					indices[j] = meshIndices[j];
				}

				IndexBufferPtr indexBuffer = VCreateIndexBuffer(usageHint, IndexBufferDatatype::UnsignedInt, indices.size() * sizeof(unsigned int));
				indexBuffer->VCopyFromSystemMemory(&(indices[0]), indices.size() * sizeof(unsigned int));
				meshBuffers->IndexBuffer = indexBuffer;
			}
			else
			{
				LOG_ASSERT(false, "mesh.Indices.Datatype is not supported.");
			}
		}
			
		for (auto shaderAttribute = shaderAttributes.begin(); shaderAttribute != shaderAttributes.end(); ++shaderAttribute)
		{
			VertexAttributePtr attribute = mesh.GetAttribute(shaderAttribute->first);
			if (attribute.get() == nullptr)
			{
				LOG_ERROR("Shader requires vertex attribute \"%s\", which is not present in mesh.", shaderAttribute->first);
			}

			if (attribute->Type == VertexAttributeType::UnsignedByte)
			{
				unsigned int count = (std::dynamic_pointer_cast<VertexAttribute<unsigned char>>(attribute))->Values.size();

				VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(unsigned char) * count);
				vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<unsigned char>>(attribute))->Values[0]), 0, sizeof(unsigned char) * count);

				meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::UnsignedByte, 1)));
			}
			else if (attribute->Type == VertexAttributeType::Float)
			{
				unsigned int count = (std::dynamic_pointer_cast<VertexAttribute<float>>(attribute))->Values.size();

				VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(float) * count);
				vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<float>>(attribute))->Values[0]), 0, sizeof(float) * count);

				meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 1)));
			}
			else if (attribute->Type == VertexAttributeType::FloatVector2)
			{
				unsigned int count = (std::dynamic_pointer_cast<VertexAttribute<Vector2<float>>>(attribute))->Values.size();

				VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector2<float>) * count);
				vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector2<float>>>(attribute))->Values[0]), 0, sizeof(Vector2<float>) * count);

				meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 2)));
			}
			else if (attribute->Type == VertexAttributeType::FloatVector3)
			{
				unsigned int count = (std::dynamic_pointer_cast<VertexAttribute<Vector3<float>>>(attribute))->Values.size();

				VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector3<float>) * count);
				vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector3<float>>>(attribute))->Values[0]), 0, sizeof(Vector3<float>) * count);

				meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 3)));
			}
			else if (attribute->Type == VertexAttributeType::FloatVector4)
			{
				unsigned int count = (std::dynamic_pointer_cast<VertexAttribute<Vector4<float>>>(attribute))->Values.size();

				VertexBufferPtr vertexBuffer = VCreateVertexBuffer(usageHint, sizeof(Vector4<float>) * count);
				vertexBuffer->VCopyFromSystemMemory(&((std::dynamic_pointer_cast<VertexAttribute<Vector4<float>>>(attribute))->Values[0]), 0, sizeof(Vector4<float>) * count);

				meshBuffers->SetAttribute(shaderAttribute->second->Location, VertexBufferAttributePtr(new VertexBufferAttribute(vertexBuffer, ComponentDatatype::Float, 4)));
			}
			else
			{
				LOG_ERROR("attribute.Datatype not implemented!");
			}
		}

		return MeshBufferPtr(meshBuffers);
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

	Texture2DPtr OGLRenderDevice::VCreateTexture2D(Texture2DDescription description)
	{
		return OGLTexture2DPtr(new OGLTexture2D(description, GL_TEXTURE_2D));
	}

	Texture2DPtr OGLRenderDevice::VCreateTexture2D(ImagePtr image, TextureFormat format)
	{
		return OGLTexture2DPtr(nullptr);
	}

	TextureSamplerPtr OGLRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
	{
		return OGLTextureSamplerPtr(new OGLTextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy));
	}

}
