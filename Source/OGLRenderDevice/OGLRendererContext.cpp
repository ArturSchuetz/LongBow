#include "OGLRenderContext.h"
#include "BowClearState.h"
#include "OGLTypeConverter.h"

#include "OGLTexture2D.h"
#include "OGLFramebuffer.h"

#include <GL/wglew.h>

namespace Bow {

	void Enable(GLenum enableCap, bool enable)
	{
		FN("Enable()");
		if (enable)
		{
			glEnable(enableCap);
		}
		else
		{
			glDisable(enableCap);
		}
	}


	OGLRenderContext::OGLRenderContext(void)
	{
		FN("OGLRenderContext::OGLRenderContext()");
	}


	OGLRenderContext::~OGLRenderContext(void) 
	{ 
		FN("OGLRenderContext::~OGLRenderContext()");
		VRelease(); 
	}


	bool OGLRenderContext::VInitialize(HWND hWnd, int width, int height)
	{
		FN("OGLRenderContext::VInitialize()");

		m_hWnd = hWnd;
		m_hdc = GetDC(hWnd);
		if (m_hdc == NULL)
		{
			LOG_ERROR("Could not fetch Device Context!");
			return false;
		}

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int pixel_format = ChoosePixelFormat(m_hdc, &pfd);
		if (pixel_format == 0)
		{
			LOG_ERROR("Could not Choose PixelFormat!");
			return false;
		}

		if (SetPixelFormat(m_hdc, pixel_format, &pfd) == FALSE)
		{
			LOG_ERROR("Could not Set PixelFormat!");
			return false;
		}

		HGLRC tempContext = wglCreateContext(m_hdc);
		if (tempContext == NULL)
		{
			LOG_ERROR("Could not Create Context!");
			return false;
		}

		// Activate the rendering context.
		if (wglMakeCurrent(m_hdc, tempContext) == FALSE)
		{
			LOG_ERROR("Could not make Context as Current!");
			return false;
		}

		GLenum error = glewInit();
		if(error != GLEW_OK)
		{
			LOG_ERROR("Init glew failed!");
			return false;
		}

		//Or better yet, use the GL3 way to get the version number
		int OpenGLVersion[2];
		glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, OpenGLVersion[0],
			WGL_CONTEXT_MINOR_VERSION_ARB, OpenGLVersion[1],
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
			0
		};

		if(wglewIsSupported("WGL_ARB_create_context") == 1)
		{
			m_hrc = wglCreateContextAttribsARB(m_hdc, NULL, attribs);
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(tempContext);
			wglMakeCurrent(m_hdc, m_hrc);
			LOG_INIT("Initialized OpenGL!");
		}
		else
		{	
			//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
			LOG_ERROR("WGL_ARB_create_context is not Supported! Using OpenGL 2.1 and before instead.");
			m_hrc = tempContext;
		}
 
		//Checking GL version
		const GLubyte *GLVersionString = glGetString(GL_VERSION);
		LOG("GL_VERSION: %s", GLVersionString);
 
		if (!m_hrc)
		{
			LOG_ERROR("Render Context Init failed.");
			return false;
		}
		LOG_INIT("Successfully Initialized");

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

		m_width		= width;
		m_height	= height;
		VSetViewport(Viewport(0, 0, m_width, m_height));

		return true;
	}


	void OGLRenderContext::VRelease(void)
	{
		FN("OGLRenderContext::VRelease()");
		wglMakeCurrent(m_hdc, 0); // Remove the rendering context from our device context  

		if (m_hrc != NULL)
		{
			wglMakeCurrent(NULL, NULL); 
			wglDeleteContext(m_hrc); // Delete our rendering context  
			m_hrc = NULL;
		}

		if (m_hdc != NULL)
		{
			ReleaseDC(m_hWnd, m_hdc); // Release the device context from our window  
			m_hdc = NULL;
		}
	}


	void OGLRenderContext::VMakeCurrent()
	{
		FN("OGLRenderContext::VMakeCurrent()");
		wglMakeCurrent(m_hdc, m_hrc);
	}
	

	VertexArrayPtr OGLRenderContext::CreateVertexArray()
	{
		FN("OGLRenderContext::CreateVertexArray()");
		return VertexArrayPtr(new OGLVertexArray());
	}

	FramebufferPtr OGLRenderContext::CreateFramebuffer()
	{
		FN("OGLRenderContext::CreateFramebuffer()");
		return OGLFramebufferPtr(new OGLFramebuffer());
	}

	void OGLRenderContext::VClear(ClearState clearState)
	{
		FN("OGLRenderContext::VClear()");
		ApplyFramebuffer();

		ApplyScissorTest(clearState.ScissorTest);
		ApplyColorMask(clearState.ColorMask);
		ApplyDepthMask(clearState.DepthMask);
		// TODO: StencilMaskSeparate
		
		if (m_clearColor != clearState.color)
		{
			glClearColor(clearState.color[0], clearState.color[1], clearState.color[2], clearState.color[3]);
			memcpy(&m_clearColor, clearState.color, sizeof(float)*4);
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
		FN("OGLRenderContext::VDraw()");
		ApplyRenderState(renderState);
        ApplyVertexArray(vertexArray);
        ApplyShaderProgram(shaderProgram);

		m_textureUnits->Clean();
		ApplyFramebuffer();

		OGLVertexArrayPtr oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(vertexArray);
		OGLIndexBufferPtr oglIndexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(oglVertexArray->GetIndexBuffer());

        if (oglIndexBuffer != nullptr)
        {
			LOG_ASSERT(false, "Not Implementet");
            //glDrawRangeElements(OGLTypeConverter::To(primitiveType), 0, oglVertexArray->MaximumArrayIndex(), count, OGLTypeConverter::To(oglIndexBuffer->GetDatatype()), offset * VertexArraySizes::SizeOf(oglIndexBuffer->GetDatatype()));
        }
        else
        {
            glDrawArrays(OGLTypeConverter::To(primitiveType), 0, std::dynamic_pointer_cast<OGLVertexArray>(vertexArray)->MaximumArrayIndex() + 1);
        }
	}

	void OGLRenderContext::VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState)
	{
		FN("OGLRenderContext::VDraw()");
		ApplyRenderState(renderState);
        ApplyVertexArray(vertexArray);
        ApplyShaderProgram(shaderProgram);

		m_textureUnits->Clean();
		ApplyFramebuffer();

		OGLVertexArrayPtr oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(vertexArray);
		OGLIndexBufferPtr oglIndexBuffer = std::dynamic_pointer_cast<OGLIndexBuffer>(oglVertexArray->GetIndexBuffer());

        if (oglIndexBuffer != nullptr)
        {
			LOG_ASSERT(false, "Not Implementet");
            //glDrawRangeElements(OGLTypeConverter::To(primitiveType), 0, oglVertexArray->MaximumArrayIndex(), count, OGLTypeConverter::To(oglIndexBuffer->GetDatatype()), offset * VertexArraySizes::SizeOf(oglIndexBuffer->GetDatatype()));
        }
        else
        {
            glDrawArrays(OGLTypeConverter::To(primitiveType), offset, count);
        }
	}

	void OGLRenderContext::VSetTexture(int location, Texture2DPtr texture)
	{
		FN("OGLRenderContext::VSetTexture()");
		LOG_ASSERT(location < m_textureUnits->GetMaxTextureUnits(), "TextureUnit does not Exist");

		m_textureUnits->SetTexture(location, std::dynamic_pointer_cast<OGLTexture2D>(texture));
	}

	void OGLRenderContext::VSetTextureSampler(int location, TextureSamplerPtr sampler)
	{
		FN("OGLRenderContext::VSetTextureSampler()");
		m_textureUnits->SetSampler(location, std::dynamic_pointer_cast<OGLTextureSampler>(sampler));
	}

	void OGLRenderContext::VSetFramebuffer(FramebufferPtr framebufer)
	{
		FN("OGLRenderContext::VSetFramebuffer()");
		m_setFramebuffer = std::dynamic_pointer_cast<OGLFramebuffer>(framebufer);
	}

	Viewport OGLRenderContext::VGetViewport(void)
	{
		return m_viewport;
	}

	void OGLRenderContext::VSetViewport(Viewport viewport)
	{
		LOG_ASSERT(!(viewport.width < 0 || viewport.height < 0),  "The viewport width and height must be greater than or equal to zero.");

        if (m_viewport != viewport)
        {
            m_viewport = viewport;
			glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
        }
	}

	void OGLRenderContext::VSwapBuffers(void)
	{
		FN("OGLRenderContext::VSwapBuffers()");
		SwapBuffers(m_hdc);
	}
	
	/*
	void OGLRenderContext::VerifyDraw(DrawState drawState, SceneState sceneState)
    {
        if (drawState == null)
        {
            throw new ArgumentNullException("drawState");
        }

        if (drawState.RenderState == null)
        {
            throw new ArgumentNullException("drawState.RenderState");
        }

        if (drawState.ShaderProgram == null)
        {
            throw new ArgumentNullException("drawState.ShaderProgram");
        }

        if (drawState.VertexArray == null)
        {
            throw new ArgumentNullException("drawState.VertexArray");
        }

        if (sceneState == null)
        {
            throw new ArgumentNullException("sceneState");
        }

        if (_setFramebuffer != null)
        {
            if (drawState.RenderState.DepthTest.Enabled &&
                !((_setFramebuffer.DepthAttachment != null) || 
                    (_setFramebuffer.DepthStencilAttachment != null)))
            {
                throw new ArgumentException("The depth test is enabled (drawState.RenderState.DepthTest.Enabled) but the context's Framebuffer property doesn't have a depth or depth/stencil attachment (DepthAttachment or DepthStencilAttachment).", "drawState");
            }
        }
    }
	*/

	void OGLRenderContext::ApplyRenderState(RenderState renderState)
	{
		FN("OGLRenderContext::ApplyRenderState()");
		ApplyPrimitiveRestart(renderState.PrimitiveRestart);
		ApplyFacetCulling(renderState.FacetCulling);
		ApplyProgramPointSize(renderState.ProgramPointSize);
		ApplyRasterizationMode(renderState.RasterizationMode);
		ApplyScissorTest(renderState.ScissorTest);
		ApplyStencilTest(renderState.StencilTest);
		ApplyDepthTest(renderState.DepthTest);
		ApplyDepthRange(renderState.DepthRange);
		ApplyBlending(renderState.Blending);
		ApplyColorMask(renderState.ColorMask);
		ApplyDepthMask(renderState.DepthMask);
	}


	void OGLRenderContext::ApplyVertexArray(VertexArrayPtr vertexArray)
    {
		FN("OGLRenderContext::ApplyVertexArray()");
		OGLVertexArrayPtr oglVertexArray = std::dynamic_pointer_cast<OGLVertexArray>(vertexArray);
        oglVertexArray->Bind();
        oglVertexArray->Clean();
    }


    void OGLRenderContext::ApplyShaderProgram(ShaderProgramPtr shaderProgram)
    {
		FN("OGLRenderContext::ApplyShaderProgram()");
        OGLShaderProgramPtr oglShaderProgram = std::dynamic_pointer_cast<OGLShaderProgram>(shaderProgram);

        if (m_boundShaderProgram != oglShaderProgram)
        {
            oglShaderProgram->Bind();
            m_boundShaderProgram = oglShaderProgram;
        }
        m_boundShaderProgram->Clean();

#if _DEBUG
		glValidateProgram(m_boundShaderProgram->GetProgram()->GetValue());

        int validateStatus;
		glGetProgramiv(m_boundShaderProgram->GetProgram()->GetValue(), GL_VALIDATE_STATUS, &validateStatus);
		if(validateStatus == 0)
		{
			LOG(LOG_COLOR_ERROR, "ErrorLog: %s", m_boundShaderProgram->GetLog().c_str());
			LOG_ASSERT(false, "Shader program validation failed");
		}
#endif
    }


	void OGLRenderContext::ForceApplyRenderState(RenderState renderState)
	{
		FN("OGLRenderContext::ForceApplyRenderState()");
		Enable(GL_PRIMITIVE_RESTART, renderState.PrimitiveRestart.Enabled);
		glPrimitiveRestartIndex(renderState.PrimitiveRestart.Index);

		Enable(GL_CULL_FACE, renderState.FacetCulling.Enabled);
		glCullFace(OGLTypeConverter::To(renderState.FacetCulling.Face));
		glFrontFace(OGLTypeConverter::To(renderState.FacetCulling.FrontFaceWindingOrder));

		Enable(GL_PROGRAM_POINT_SIZE, renderState.ProgramPointSize == ProgramPointSize::Enabled);
		glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(renderState.RasterizationMode));

		Enable(GL_SCISSOR_TEST, renderState.ScissorTest.Enabled);
		RECT rectangle = renderState.ScissorTest.rectangle;
		glScissor(rectangle.left, rectangle.top, rectangle.right-rectangle.left, rectangle.bottom-rectangle.top);

		Enable(GL_STENCIL_TEST, renderState.StencilTest.Enabled);
	
		ForceApplyRenderStateStencil(StencilFace::Front, renderState.StencilTest.FrontFace);
		ForceApplyRenderStateStencil(StencilFace::Back, renderState.StencilTest.BackFace);

		Enable(GL_DEPTH_TEST, renderState.DepthTest.Enabled);
		glDepthFunc(OGLTypeConverter::To(renderState.DepthTest.Function));

		glDepthRange(renderState.DepthRange.Near, renderState.DepthRange.Far);

		Enable(GL_BLEND, renderState.Blending.Enabled);
		glBlendFuncSeparate(
			OGLTypeConverter::To(renderState.Blending.SourceRGBFactor),
			OGLTypeConverter::To(renderState.Blending.DestinationRGBFactor),
			OGLTypeConverter::To(renderState.Blending.SourceAlphaFactor),
			OGLTypeConverter::To(renderState.Blending.DestinationAlphaFactor));
		glBlendEquationSeparate(
			OGLTypeConverter::To(renderState.Blending.RGBEquation),
			OGLTypeConverter::To(renderState.Blending.AlphaEquation));


		glBlendColor(renderState.Blending.color[0], renderState.Blending.color[1], renderState.Blending.color[2], renderState.Blending.color[3]);

		glDepthMask(renderState.DepthMask);

		glColorMask(renderState.ColorMask.GetRed(), renderState.ColorMask.GetGreen(), renderState.ColorMask.GetBlue(), renderState.ColorMask.GetAlpha());
	}


	void OGLRenderContext::ForceApplyRenderStateStencil(StencilFace face, StencilTestFace test)
	{
		FN("OGLRenderContext::ForceApplyRenderStateStencil()");
		glStencilOpSeparate((GLenum)face,
			OGLTypeConverter::To(test.StencilFailOperation),
			OGLTypeConverter::To(test.DepthFailStencilPassOperation),
			OGLTypeConverter::To(test.DepthPassStencilPassOperation));

		glStencilFuncSeparate((GLenum)face,
			OGLTypeConverter::To(test.Function),
			test.ReferenceValue,
			test.Mask);
	}


	void OGLRenderContext::ApplyPrimitiveRestart(PrimitiveRestart primitiveRestart)
	{
		FN("OGLRenderContext::ApplyPrimitiveRestart()");
		if (m_renderState.PrimitiveRestart.Enabled != primitiveRestart.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set PrimitiveRestart Enabled: %d", primitiveRestart.Enabled);
			Enable(GL_PRIMITIVE_RESTART, primitiveRestart.Enabled);
			m_renderState.PrimitiveRestart.Enabled = primitiveRestart.Enabled;
		}

		if (primitiveRestart.Enabled)
		{
			if (m_renderState.PrimitiveRestart.Index != primitiveRestart.Index)
			{
				glPrimitiveRestartIndex(primitiveRestart.Index);
			   m_renderState.PrimitiveRestart.Index = primitiveRestart.Index;
			}
		}
	}


	void OGLRenderContext::ApplyFacetCulling(FacetCulling facetCulling)
	{
		FN("OGLRenderContext::ApplyFacetCulling()");
		if (m_renderState.FacetCulling.Enabled != facetCulling.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set FacetCulling Enabled: %d", facetCulling.Enabled);
			Enable(GL_CULL_FACE, facetCulling.Enabled);
			m_renderState.FacetCulling.Enabled = facetCulling.Enabled;
		}

		if (facetCulling.Enabled)
		{
			if (m_renderState.FacetCulling.Face != facetCulling.Face)
			{
				glCullFace(OGLTypeConverter::To(facetCulling.Face));
			   m_renderState.FacetCulling.Face = facetCulling.Face;
			}

			if (m_renderState.FacetCulling.FrontFaceWindingOrder != facetCulling.FrontFaceWindingOrder)
			{
				glFrontFace(OGLTypeConverter::To(facetCulling.FrontFaceWindingOrder));
			   m_renderState.FacetCulling.FrontFaceWindingOrder = facetCulling.FrontFaceWindingOrder;
			}
		}
	}


	void OGLRenderContext::ApplyProgramPointSize(ProgramPointSize programPointSize)
	{
		FN("OGLRenderContext::ApplyProgramPointSize()");
		if (m_renderState.ProgramPointSize != programPointSize)
		{
			Enable(GL_PROGRAM_POINT_SIZE, programPointSize == ProgramPointSize::Enabled);
		   m_renderState.ProgramPointSize = programPointSize;
		}
	}


	void OGLRenderContext::ApplyRasterizationMode(RasterizationMode rasterizationMode)
	{
		FN("OGLRenderContext::ApplyRasterizationMode()");
		if (m_renderState.RasterizationMode != rasterizationMode)
		{
			glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(rasterizationMode));
		   m_renderState.RasterizationMode = rasterizationMode;
		}
	}


	void OGLRenderContext::ApplyScissorTest(ScissorTest scissorTest)
	{
		FN("OGLRenderContext::ApplyScissorTest()");
		RECT rectangle = scissorTest.rectangle;

		if (rectangle.right-rectangle.left < 0)
		{
			LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Width must be greater than or equal to zero");
			return;
		}

		if (rectangle.bottom-rectangle.top < 0)
		{
			LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Height must be greater than or equal to zero");
			return;
		}

		if (m_renderState.ScissorTest.Enabled != scissorTest.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set ScissorTest Enabled: %d", scissorTest.Enabled);
			Enable(GL_SCISSOR_TEST, scissorTest.Enabled);
			m_renderState.ScissorTest.Enabled = scissorTest.Enabled;
		}

		if (scissorTest.Enabled)
		{
			if (!(m_renderState.ScissorTest.rectangle.bottom == scissorTest.rectangle.bottom && m_renderState.ScissorTest.rectangle.left == scissorTest.rectangle.left &&m_renderState.ScissorTest.rectangle.right == scissorTest.rectangle.right &&m_renderState.ScissorTest.rectangle.top == scissorTest.rectangle.top))
			{
				glScissor(rectangle.left, rectangle.bottom, rectangle.right-rectangle.left, rectangle.bottom-rectangle.top);
			   m_renderState.ScissorTest.rectangle = scissorTest.rectangle;
			}
		}
	}


	void OGLRenderContext::ApplyStencilTest(StencilTest stencilTest)
	{
		FN("OGLRenderContext::ApplyStencilTest()");
		if (m_renderState.StencilTest.Enabled != stencilTest.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set StencilTest Enabled: %d", stencilTest.Enabled);
			Enable(GL_STENCIL_TEST, stencilTest.Enabled);
			m_renderState.StencilTest.Enabled = stencilTest.Enabled;
		}

		if (stencilTest.Enabled)
		{
			ApplyStencil(StencilFace::Front, m_renderState.StencilTest.FrontFace, stencilTest.FrontFace);
			ApplyStencil(StencilFace::Back, m_renderState.StencilTest.BackFace, stencilTest.BackFace);
		}
	}


	void OGLRenderContext::ApplyStencil(StencilFace face, StencilTestFace currentTest, StencilTestFace test)
	{
		FN("OGLRenderContext::ApplyStencil()");
		if ((currentTest.StencilFailOperation != test.StencilFailOperation) ||
			(currentTest.DepthFailStencilPassOperation != test.DepthFailStencilPassOperation) ||
			(currentTest.DepthPassStencilPassOperation != test.DepthPassStencilPassOperation))
		{
			glStencilOpSeparate((GLenum)face,
				OGLTypeConverter::To(test.StencilFailOperation),
				OGLTypeConverter::To(test.DepthFailStencilPassOperation),
				OGLTypeConverter::To(test.DepthPassStencilPassOperation));

			currentTest.StencilFailOperation = test.StencilFailOperation;
			currentTest.DepthFailStencilPassOperation = test.DepthFailStencilPassOperation;
			currentTest.DepthPassStencilPassOperation = test.DepthPassStencilPassOperation;
		}

		if ((currentTest.Function != test.Function) ||
			(currentTest.ReferenceValue != test.ReferenceValue) ||
			(currentTest.Mask != test.Mask))
		{
			glStencilFuncSeparate((GLenum)face,
				OGLTypeConverter::To(test.Function),
				test.ReferenceValue,
				test.Mask);

			currentTest.Function = test.Function;
			currentTest.ReferenceValue = test.ReferenceValue;
			currentTest.Mask = test.Mask;
		}
	}


	void OGLRenderContext::ApplyDepthTest(DepthTest depthTest)
	{
		FN("OGLRenderContext::ApplyDepthTest()");
		if (m_renderState.DepthTest.Enabled != depthTest.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set DepthTest Enabled: %d", depthTest.Enabled);
			Enable(GL_DEPTH_TEST, depthTest.Enabled);
			m_renderState.DepthTest.Enabled = depthTest.Enabled;
		}

		if (depthTest.Enabled)
		{
			if (m_renderState.DepthTest.Function != depthTest.Function)
			{
				glDepthFunc(OGLTypeConverter::To(depthTest.Function));
			   m_renderState.DepthTest.Function = depthTest.Function;
			}
		}
	}


	void OGLRenderContext::ApplyDepthRange(DepthRange depthRange)
	{
		FN("OGLRenderContext::ApplyDepthRange()");
		if (depthRange.Near < 0.0 || depthRange.Near > 1.0)
		{
			LOG_ASSERT(false, "renderState.DepthRange.Near must be between zero and one");
			return;
		}

		if (depthRange.Far < 0.0 || depthRange.Far > 1.0)
		{
			LOG_ASSERT(false, "renderState.DepthRange.Far must be between zero and one");
			return;
		}

		if ((m_renderState.DepthRange.Near != depthRange.Near) ||
			(m_renderState.DepthRange.Far != depthRange.Far))
		{
			glDepthRange(depthRange.Near, depthRange.Far);

			m_renderState.DepthRange.Near = depthRange.Near;
			m_renderState.DepthRange.Far = depthRange.Far;
		}
	}


	void OGLRenderContext::ApplyBlending(Blending blending)
	{
		FN("OGLRenderContext::ApplyBlending()");
		if (m_renderState.Blending.Enabled != blending.Enabled)
		{
			LOG(LOG_COLOR_INFO, "Set Blending Enabled: %d", blending.Enabled);
			Enable(GL_BLEND, blending.Enabled);
			m_renderState.Blending.Enabled = blending.Enabled;
		}

		if (blending.Enabled)
		{
			if ((m_renderState.Blending.SourceRGBFactor != blending.SourceRGBFactor) ||
				(m_renderState.Blending.DestinationRGBFactor != blending.DestinationRGBFactor) ||
				(m_renderState.Blending.SourceAlphaFactor != blending.SourceAlphaFactor) ||
				(m_renderState.Blending.DestinationAlphaFactor != blending.DestinationAlphaFactor))
			{
				glBlendFuncSeparate(
					OGLTypeConverter::To(blending.SourceRGBFactor),
					OGLTypeConverter::To(blending.DestinationRGBFactor),
					OGLTypeConverter::To(blending.SourceAlphaFactor),
					OGLTypeConverter::To(blending.DestinationAlphaFactor));

			   m_renderState.Blending.SourceRGBFactor = blending.SourceRGBFactor;
			   m_renderState.Blending.DestinationRGBFactor = blending.DestinationRGBFactor;
			   m_renderState.Blending.SourceAlphaFactor = blending.SourceAlphaFactor;
			   m_renderState.Blending.DestinationAlphaFactor = blending.DestinationAlphaFactor;
			}

			if ((m_renderState.Blending.RGBEquation != blending.RGBEquation) ||
				(m_renderState.Blending.AlphaEquation != blending.AlphaEquation))
			{
				glBlendEquationSeparate(
					OGLTypeConverter::To(blending.RGBEquation),
					OGLTypeConverter::To(blending.AlphaEquation));

			   m_renderState.Blending.RGBEquation = blending.RGBEquation;
			   m_renderState.Blending.AlphaEquation = blending.AlphaEquation;
			}

			if (m_renderState.Blending.color != blending.color)
			{
				LOG(LOG_COLOR_INFO, "Set BlendColor to: %d, %d, %d, %d", blending.color[0], blending.color[1], blending.color[2], blending.color[3]);
				glBlendColor(blending.color[0], blending.color[1], blending.color[2], blending.color[3]);
				memcpy(&m_renderState.Blending.color, &blending.color, sizeof(float)*4);
			}
		}
	}


	void OGLRenderContext::ApplyColorMask(ColorMask colorMask)
	{
		FN("OGLRenderContext::ApplyColorMask()");
		if (m_renderState.ColorMask != colorMask)
		{
			LOG(LOG_COLOR_INFO, "Set ColorMask to: %d, %d, %d, %d", colorMask.GetRed(), colorMask.GetGreen(), colorMask.GetBlue(), colorMask.GetAlpha());
			glColorMask(colorMask.GetRed(), colorMask.GetGreen(), colorMask.GetBlue(), colorMask.GetAlpha());
			m_renderState.ColorMask = colorMask;
		}
	}


	void OGLRenderContext::ApplyDepthMask(bool depthMask)
	{
		FN("OGLRenderContext::ApplyDepthMask()");
		if (m_renderState.DepthMask != depthMask)
		{
			LOG(LOG_COLOR_INFO, "Set DepthMask to: %d", depthMask);
			glDepthMask(depthMask);
			m_renderState.DepthMask = depthMask;
		}
	}


	void OGLRenderContext::ApplyFramebuffer()
	{
		FN("OGLRenderContext::ApplyFramebuffer()");
	
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