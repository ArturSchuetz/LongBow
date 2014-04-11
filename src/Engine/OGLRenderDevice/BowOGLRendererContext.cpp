#include "BowOGLRenderContext.h"
#include "BowLogger.h"
#include "BowOGLTypeConverter.h"

#include "BowClearState.h"

#include "BowOGLShaderProgram.h"

#include "BowOGLTexture2D.h"
#include "BowOGLTextureUnits.h"
#include "BowOGLTextureSampler.h"

#include "BowOGLFramebuffer.h"

#include "BowOGLIndexBuffer.h"
#include "BowOGLVertexArray.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace Bow {
	namespace Renderer {

		OGLRenderContext* OGLRenderContext::m_currentContext;

		OGLRenderContext::OGLRenderContext(GLFWwindow* window) :
			m_window(window), 
			m_initialized(false)
		{
		}


		bool OGLRenderContext::Initialize()
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			if (GLEW_OK != glewInit())
			{
				LOG_ERROR("Could not initialize GLEW!");
			}
			else
			{
				LOG_DEBUG("GLEW sucessfully initialized!");
			}

			float clearColor[4];
			glGetFloatv(GL_DEPTH_CLEAR_VALUE, &m_clearDepth);
			glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &m_clearStencil);
			glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clearColor);

			m_clearColor[0] = clearColor[0] * 255.0f;
			m_clearColor[1] = clearColor[1] * 255.0f;
			m_clearColor[2] = clearColor[2] * 255.0f;
			m_clearColor[3] = clearColor[3] * 255.0f;

			m_textureUnits = OGLTextureUnitsPtr(new OGLTextureUnits());

			//
			// Sync GL state with default render state.
			//
			ForceApplyRenderState(m_renderState);

			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);
			VSetViewport(Viewport(0, 0, width, height));

			LOG_DEBUG("OpenGL-Context sucessfully initialized!");

			//Checking GL version
			const GLubyte *GLVersionString = glGetString(GL_VERSION);
			LOG_DEBUG("Using GL_VERSION: %s", GLVersionString);

			m_initialized = true;
			return m_initialized;
		}


		OGLRenderContext::~OGLRenderContext(void)
		{
			VRelease();
		}


		void OGLRenderContext::VRelease(void)
		{
			m_initialized = false;
			m_window = nullptr;
			LOG_DEBUG("OGLRenderContext released");
		}


		VertexArrayPtr OGLRenderContext::CreateVertexArray()
		{
			return VertexArrayPtr(new OGLVertexArray());
		}


		FramebufferPtr OGLRenderContext::CreateFramebuffer()
		{
			return OGLFramebufferPtr(new OGLFramebuffer());
		}


		void OGLRenderContext::VClear(ClearState clearState)
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			ApplyFramebuffer();

			ApplyScissorTest(clearState.ScissorTest);
			ApplyColorMask(clearState.ColorMask);
			ApplyDepthMask(clearState.DepthMask);
			// TODO: StencilMaskSeparate

			if (m_clearColor != clearState.Color)
			{
				glClearColor(clearState.Color[0], clearState.Color[1], clearState.Color[2], clearState.Color[3]);
				memcpy(&m_clearColor, clearState.Color, sizeof(float)* 4);
			}

			if (m_clearDepth != clearState.Depth)
			{
				glClearDepth((double)clearState.Depth);
				m_clearDepth = clearState.Depth;
			}

			if (m_clearStencil != clearState.Stencil)
			{
				glClearStencil(clearState.Stencil);
				m_clearStencil = clearState.Stencil;
			}

			glClear(OGLTypeConverter::To(clearState.Buffers));
		}


		void OGLRenderContext::VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			ApplyRenderState(renderState);
			ApplyVertexArray(vertexArray);
			ApplyShaderProgram(shaderProgram);

			Draw(primitiveType, 0, std::dynamic_pointer_cast<OGLVertexArray>(vertexArray)->MaximumArrayIndex() + 1, vertexArray, shaderProgram, renderState);
		}


		void OGLRenderContext::VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			ApplyRenderState(renderState);
			ApplyVertexArray(vertexArray);
			ApplyShaderProgram(shaderProgram);

			Draw(primitiveType, offset, count, vertexArray, shaderProgram, renderState);
		}

		void OGLRenderContext::Draw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
		{
			m_textureUnits->Clean();

			glFlush();
			ApplyFramebuffer();

			OGLVertexArrayPtr oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(vertexArray);
			OGLIndexBufferPtr oglIndexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(oglVertexArray->GetIndexBuffer());

			if (oglIndexBuffer != nullptr)
			{
				LOG_FATAL("Not Implementet");
				//glDrawRangeElements(OGLTypeConverter::To(primitiveType), 0, oglVertexArray->MaximumArrayIndex(), count, OGLTypeConverter::To(oglIndexBuffer->GetDatatype()), offset * VertexArraySizes::SizeOf(oglIndexBuffer->GetDatatype()));
			}
			else
			{
				glDrawArrays(OGLTypeConverter::To(primitiveType), offset, count);
			}
		}


		void OGLRenderContext::VSetTexture(int location, Texture2DPtr texture)
		{
			LOG_ASSERT(location < m_textureUnits->GetMaxTextureUnits(), "TextureUnit does not Exist");

			m_textureUnits->SetTexture(location, std::dynamic_pointer_cast<OGLTexture2D>(texture));
		}


		void OGLRenderContext::VSetTextureSampler(int location, TextureSamplerPtr sampler)
		{
			m_textureUnits->SetSampler(location, std::dynamic_pointer_cast<OGLTextureSampler>(sampler));
		}


		void OGLRenderContext::VSetFramebuffer(FramebufferPtr framebufer)
		{
			m_setFramebuffer = std::dynamic_pointer_cast<OGLFramebuffer>(framebufer);
		}


		void OGLRenderContext::VSetViewport(Viewport viewport)
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			LOG_ASSERT(!(viewport.width < 0 || viewport.height < 0), "The viewport width and height must be greater than or equal to zero.");

			if (m_viewport != viewport)
			{
				m_viewport = viewport;
				glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
			}
		}


		Viewport OGLRenderContext::VGetViewport(void)
		{
			return m_viewport;
		}


		void OGLRenderContext::VSwapBuffers(void)
		{
			if (m_currentContext != this)
			{
				glfwMakeContextCurrent(m_window);
				m_currentContext = this;
			}

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}


		void OGLRenderContext::ApplyVertexArray(VertexArrayPtr vertexArray)
		{
			OGLVertexArrayPtr oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(vertexArray);
			oglVertexArray->Bind();
			oglVertexArray->Clean();
		}


		void OGLRenderContext::ApplyShaderProgram(ShaderProgramPtr shaderProgram)
		{
			OGLShaderProgramPtr oglShaderProgram = std::dynamic_pointer_cast<OGLShaderProgram>(shaderProgram);

			if (m_boundShaderProgram != oglShaderProgram)
			{
				oglShaderProgram->Bind();
				m_boundShaderProgram = oglShaderProgram;
			}
			m_boundShaderProgram->Clean();

#if _DEBUG
			glValidateProgram(m_boundShaderProgram->GetProgram());

			int validateStatus;
			glGetProgramiv(m_boundShaderProgram->GetProgram(), GL_VALIDATE_STATUS, &validateStatus);
			if (validateStatus == 0)
			{
				LOG_ERROR("Shader program validation failed: %s", m_boundShaderProgram->GetLog().c_str());
			}
#endif
		}


		void OGLRenderContext::ApplyFramebuffer()
		{
			if (m_boundFramebuffer != m_setFramebuffer)
			{
				if (m_setFramebuffer != nullptr)
				{
					m_setFramebuffer->Bind();
				}
				else
				{
					OGLFramebuffer::UnBind();
				}

				m_boundFramebuffer = m_setFramebuffer;
			}

			if (m_setFramebuffer != nullptr)
			{
				m_setFramebuffer->Clean();
#if _DEBUG
				GLenum errorCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				LOG_ASSERT(!(errorCode != GL_FRAMEBUFFER_COMPLETE), "Frame buffer is incomplete.");
#endif
			}
		}

	}
}