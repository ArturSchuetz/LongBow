#include "BowOGL3xRenderContext.h"
#include "BowLogger.h"
#include "BowOGL3xTypeConverter.h"

#include "BowClearState.h"

#include "BowOGL3xShaderProgram.h"

#include "BowOGL3xTexture2D.h"
#include "BowOGL3xTextureUnits.h"
#include "BowOGL3xTextureSampler.h"

#include "BowOGL3xFramebuffer.h"

#include "BowOGL3xIndexBuffer.h"
#include "BowOGL3xVertexArray.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace bow {

	enum class StencilFace : GLenum
	{
		Front = GL_FRONT,
		Back = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK
	};

	enum class MaterialFace : GLenum
	{
		Front = GL_FRONT,
		Back = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK
	};


	void Enable(GLenum enableCap, bool enable)
	{
		if (enable)
		{
			glEnable(enableCap);
		}
		else
		{
			glDisable(enableCap);
		}
	}


	void OGLRenderContext::ForceApplyRenderState(RenderState renderState)
	{
		glPointSize(renderState.PointSize);
		glPointSize(renderState.LineWidth);

		Enable(GL_PRIMITIVE_RESTART, renderState.PrimitiveRestart.Enabled);
		glPrimitiveRestartIndex(renderState.PrimitiveRestart.Index);

		Enable(GL_CULL_FACE, renderState.FaceCulling.Enabled);
		glCullFace(OGLTypeConverter::To(renderState.FaceCulling.Face));
		glFrontFace(OGLTypeConverter::To(renderState.FaceCulling.FrontFaceWindingOrder));

		Enable(GL_PROGRAM_POINT_SIZE, renderState.ProgramPointSize == ProgramPointSize::Enabled);
		glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(renderState.RasterizationMode));

		Enable(GL_SCISSOR_TEST, renderState.ScissorTest.Enabled);
		RECT rectangle = renderState.ScissorTest.rectangle;
		glScissor(rectangle.left, rectangle.top, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top);

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
		glStencilOpSeparate((GLenum)face,
			OGLTypeConverter::To(test.StencilFailOperation),
			OGLTypeConverter::To(test.DepthFailStencilPassOperation),
			OGLTypeConverter::To(test.DepthPassStencilPassOperation));

		glStencilFuncSeparate((GLenum)face,
			OGLTypeConverter::To(test.Function),
			test.ReferenceValue,
			test.Mask);
	}


	void OGLRenderContext::ApplyRenderState(RenderState renderState)
	{
		if (m_renderState.PointSize != renderState.PointSize)
		{
			m_renderState.PointSize = renderState.PointSize;
			glPointSize(renderState.PointSize);
		}

		if (m_renderState.LineWidth != renderState.LineWidth)
		{
			m_renderState.LineWidth = renderState.LineWidth;
			glLineWidth(renderState.LineWidth);
		}

		ApplyPrimitiveRestart(renderState.PrimitiveRestart);
		ApplyFaceCulling(renderState.FaceCulling);
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


	void OGLRenderContext::ApplyPrimitiveRestart(PrimitiveRestart primitiveRestart)
	{
		if (m_renderState.PrimitiveRestart.Enabled != primitiveRestart.Enabled)
		{
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


	void OGLRenderContext::ApplyFaceCulling(FaceCulling FaceCulling)
	{
		if (m_renderState.FaceCulling.Enabled != FaceCulling.Enabled)
		{
			Enable(GL_CULL_FACE, FaceCulling.Enabled);
			m_renderState.FaceCulling.Enabled = FaceCulling.Enabled;
		}

		if (FaceCulling.Enabled)
		{
			if (m_renderState.FaceCulling.Face != FaceCulling.Face)
			{
				glCullFace(OGLTypeConverter::To(FaceCulling.Face));
				m_renderState.FaceCulling.Face = FaceCulling.Face;
			}

			if (m_renderState.FaceCulling.FrontFaceWindingOrder != FaceCulling.FrontFaceWindingOrder)
			{
				glFrontFace(OGLTypeConverter::To(FaceCulling.FrontFaceWindingOrder));
				m_renderState.FaceCulling.FrontFaceWindingOrder = FaceCulling.FrontFaceWindingOrder;
			}
		}
	}


	void OGLRenderContext::ApplyProgramPointSize(ProgramPointSize programPointSize)
	{
		if (m_renderState.ProgramPointSize != programPointSize)
		{
			Enable(GL_PROGRAM_POINT_SIZE, programPointSize == ProgramPointSize::Enabled);
			m_renderState.ProgramPointSize = programPointSize;
		}
	}


	void OGLRenderContext::ApplyRasterizationMode(RasterizationMode rasterizationMode)
	{
		if (m_renderState.RasterizationMode != rasterizationMode)
		{
			glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(rasterizationMode));
			m_renderState.RasterizationMode = rasterizationMode;
		}
	}


	void OGLRenderContext::ApplyScissorTest(ScissorTest scissorTest)
	{
		RECT rectangle = scissorTest.rectangle;

		if (rectangle.right - rectangle.left < 0)
		{
			LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Width must be greater than or equal to zero");
			return;
		}

		if (rectangle.bottom - rectangle.top < 0)
		{
			LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Height must be greater than or equal to zero");
			return;
		}

		if (m_renderState.ScissorTest.Enabled != scissorTest.Enabled)
		{
			Enable(GL_SCISSOR_TEST, scissorTest.Enabled);
			m_renderState.ScissorTest.Enabled = scissorTest.Enabled;
		}

		if (scissorTest.Enabled)
		{
			if (!(m_renderState.ScissorTest.rectangle.bottom == scissorTest.rectangle.bottom && m_renderState.ScissorTest.rectangle.left == scissorTest.rectangle.left &&m_renderState.ScissorTest.rectangle.right == scissorTest.rectangle.right &&m_renderState.ScissorTest.rectangle.top == scissorTest.rectangle.top))
			{
				glScissor(rectangle.left, rectangle.bottom, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top);
				m_renderState.ScissorTest.rectangle = scissorTest.rectangle;
			}
		}
	}


	void OGLRenderContext::ApplyStencilTest(StencilTest stencilTest)
	{
		if (m_renderState.StencilTest.Enabled != stencilTest.Enabled)
		{
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
		if (m_renderState.DepthTest.Enabled != depthTest.Enabled)
		{
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
		if (m_renderState.Blending.Enabled != blending.Enabled)
		{
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
				glBlendColor(blending.color[0], blending.color[1], blending.color[2], blending.color[3]);
				memcpy(&m_renderState.Blending.color, &blending.color, sizeof(float)* 4);
			}
		}
	}

	void OGLRenderContext::ApplyColorMask(ColorMask colorMask)
	{
		if (m_renderState.ColorMask != colorMask)
		{
			glColorMask(colorMask.GetRed(), colorMask.GetGreen(), colorMask.GetBlue(), colorMask.GetAlpha());
			m_renderState.ColorMask = colorMask;
		}
	}


	void OGLRenderContext::ApplyDepthMask(bool depthMask)
	{
		if (m_renderState.DepthMask != depthMask)
		{
			glDepthMask(depthMask);
			m_renderState.DepthMask = depthMask;
		}
	}
}
