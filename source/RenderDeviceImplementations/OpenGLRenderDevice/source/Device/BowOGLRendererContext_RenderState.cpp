#include <OpenGLRenderDevice/Device/BowOGLRenderContext.h>

#include <OpenGLRenderDevice/BowOGLTypeConverter.h>
#include <OpenGLRenderDevice/Device/Buffer/BowOGLIndexBuffer.h>
#include <OpenGLRenderDevice/Device/Context/FrameBuffer/BowOGLFramebuffer.h>
#include <OpenGLRenderDevice/Device/Context/VertexAttributeBindings/BowOGLVertexAttributeBindings.h>
#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderProgram.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTexture2D.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTextureSampler.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTextureUnits.h>

#include <RenderDevice/BowClearState.h>

#include <CoreSystems/BowLogger.h>


#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

namespace bow
{

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
    FN("Enable");

    if (enable)
    {
        LOG_TRACE("glEnable");
        glEnable(enableCap);
    }
    else
    {
        LOG_TRACE("glDisable");
        glDisable(enableCap);
    }
}

void OGLRenderContext::ForceApplyRenderState(RenderState renderState)
{
    FN("OGLRenderContext::ForceApplyRenderState");

    LOG_TRACE("glPointSize");
    glPointSize(renderState.pointSize);
    LOG_TRACE("glPointSize");
    glPointSize(renderState.lineWidth);

    Enable(GL_PRIMITIVE_RESTART, renderState.primitiveRestart.Enabled);
    LOG_TRACE("glPrimitiveRestartIndex");
    glPrimitiveRestartIndex(renderState.primitiveRestart.Index);

    Enable(GL_CULL_FACE, renderState.faceCulling.Enabled);
    LOG_TRACE("glCullFace");
    glCullFace(OGLTypeConverter::To(renderState.faceCulling.Face));
    LOG_TRACE("glFrontFace");
    glFrontFace(OGLTypeConverter::To(renderState.faceCulling.FrontFaceWindingOrder));

    Enable(GL_PROGRAM_POINT_SIZE, renderState.programPointSize == ProgramPointSize::Enabled);
    LOG_TRACE("glPolygonMode");
    glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(renderState.rasterizationMode));

    Enable(GL_SCISSOR_TEST, renderState.scissorTest.Enabled);
    long rectangle_left = renderState.scissorTest.rectangle_left;
    long rectangle_top = renderState.scissorTest.rectangle_top;
    long rectangle_right = renderState.scissorTest.rectangle_right;
    long rectangle_bottom = renderState.scissorTest.rectangle_bottom;

    LOG_TRACE("glScissor");
    glScissor(rectangle_left, rectangle_top, rectangle_right - rectangle_left, rectangle_bottom - rectangle_top);

    Enable(GL_STENCIL_TEST, renderState.stencilTest.Enabled);

    ForceApplyRenderStateStencil(StencilFace::Front, renderState.stencilTest.FrontFace);
    ForceApplyRenderStateStencil(StencilFace::Back, renderState.stencilTest.BackFace);

    Enable(GL_DEPTH_TEST, renderState.depthTest.Enabled);
    LOG_TRACE("glDepthFunc");
    glDepthFunc(OGLTypeConverter::To(renderState.depthTest.Function));

    LOG_TRACE("glDepthRange");
    glDepthRange(renderState.depthRange.Near, renderState.depthRange.Far);

    Enable(GL_BLEND, renderState.blending.Enabled);
    LOG_TRACE("glBlendFuncSeparate");
    glBlendFuncSeparate(OGLTypeConverter::To(renderState.blending.SourceRGBFactor), OGLTypeConverter::To(renderState.blending.DestinationRGBFactor), OGLTypeConverter::To(renderState.blending.SourceAlphaFactor),
                        OGLTypeConverter::To(renderState.blending.DestinationAlphaFactor));
    LOG_TRACE("glBlendEquationSeparate");
    glBlendEquationSeparate(OGLTypeConverter::To(renderState.blending.RGBEquation), OGLTypeConverter::To(renderState.blending.AlphaEquation));

    LOG_TRACE("glBlendColor");
    glBlendColor(renderState.blending.color[0], renderState.blending.color[1], renderState.blending.color[2], renderState.blending.color[3]);

    LOG_TRACE("glDepthMask");
    glDepthMask(renderState.depthMask);

    LOG_TRACE("glColorMask");
    glColorMask(renderState.colorMask.red, renderState.colorMask.green, renderState.colorMask.blue, renderState.colorMask.alpha);
}

void OGLRenderContext::ForceApplyRenderStateStencil(StencilFace face, StencilTestFace test)
{
    FN("OGLRenderContext::ForceApplyRenderStateStencil");

    LOG_TRACE("glStencilOpSeparate");
    glStencilOpSeparate((GLenum)face, OGLTypeConverter::To(test.StencilFailOperation), OGLTypeConverter::To(test.DepthFailStencilPassOperation), OGLTypeConverter::To(test.DepthPassStencilPassOperation));

    LOG_TRACE("glStencilFuncSeparate");
    glStencilFuncSeparate((GLenum)face, OGLTypeConverter::To(test.Function), test.ReferenceValue, test.CompareMask);
}

void OGLRenderContext::ApplyRenderState(RenderState renderState)
{
    FN("OGLRenderContext::ApplyRenderState");

    if (m_renderState.pointSize != renderState.pointSize)
    {
        m_renderState.pointSize = renderState.pointSize;
        LOG_TRACE("glPointSize");
        glPointSize(renderState.pointSize);
    }

    if (m_renderState.lineWidth != renderState.lineWidth)
    {
        m_renderState.lineWidth = renderState.lineWidth;
        LOG_TRACE("glLineWidth");
        glLineWidth(renderState.lineWidth);
    }

    ApplyPrimitiveRestart(renderState.primitiveRestart);
    ApplyFaceCulling(renderState.faceCulling);
    ApplyProgramPointSize(renderState.programPointSize);
    ApplyRasterizationMode(renderState.rasterizationMode);
    ApplyScissorTest(renderState.scissorTest);
    ApplyStencilTest(renderState.stencilTest);
    ApplyDepthTest(renderState.depthTest);
    ApplyDepthRange(renderState.depthRange);
    ApplyBlending(renderState.blending);
    ApplyColorMask(renderState.colorMask);
    ApplyDepthMask(renderState.depthMask);
}

void OGLRenderContext::ApplyPrimitiveRestart(PrimitiveRestart primitiveRestart)
{
    FN("OGLRenderContext::ApplyPrimitiveRestart");

    if (m_renderState.primitiveRestart.Enabled != primitiveRestart.Enabled)
    {
        Enable(GL_PRIMITIVE_RESTART, primitiveRestart.Enabled);
        m_renderState.primitiveRestart.Enabled = primitiveRestart.Enabled;
    }

    if (primitiveRestart.Enabled)
    {
        if (m_renderState.primitiveRestart.Index != primitiveRestart.Index)
        {
            LOG_TRACE("glPrimitiveRestartIndex");
            glPrimitiveRestartIndex(primitiveRestart.Index);
            m_renderState.primitiveRestart.Index = primitiveRestart.Index;
        }
    }
}

void OGLRenderContext::ApplyFaceCulling(FaceCulling FaceCulling)
{
    FN("OGLRenderContext::ApplyFaceCulling");

    if (m_renderState.faceCulling.Enabled != FaceCulling.Enabled)
    {
        Enable(GL_CULL_FACE, FaceCulling.Enabled);
        m_renderState.faceCulling.Enabled = FaceCulling.Enabled;
    }

    if (FaceCulling.Enabled)
    {
        if (m_renderState.faceCulling.Face != FaceCulling.Face)
        {
            LOG_TRACE("glCullFace");
            glCullFace(OGLTypeConverter::To(FaceCulling.Face));
            m_renderState.faceCulling.Face = FaceCulling.Face;
        }

        if (m_renderState.faceCulling.FrontFaceWindingOrder != FaceCulling.FrontFaceWindingOrder)
        {
            LOG_TRACE("glFrontFace");
            glFrontFace(OGLTypeConverter::To(FaceCulling.FrontFaceWindingOrder));
            m_renderState.faceCulling.FrontFaceWindingOrder = FaceCulling.FrontFaceWindingOrder;
        }
    }
}

void OGLRenderContext::ApplyProgramPointSize(ProgramPointSize programPointSize)
{
    FN("OGLRenderContext::ApplyProgramPointSize");

    if (m_renderState.programPointSize != programPointSize)
    {
        Enable(GL_PROGRAM_POINT_SIZE, programPointSize == ProgramPointSize::Enabled);
        m_renderState.programPointSize = programPointSize;
    }
}

void OGLRenderContext::ApplyRasterizationMode(RasterizationMode rasterizationMode)
{
    FN("OGLRenderContext::ApplyRasterizationMode");

    if (m_renderState.rasterizationMode != rasterizationMode)
    {
        LOG_TRACE("glPolygonMode");
        glPolygonMode((GLenum)MaterialFace::FrontAndBack, OGLTypeConverter::To(rasterizationMode));
        m_renderState.rasterizationMode = rasterizationMode;
    }
}

void OGLRenderContext::ApplyScissorTest(ScissorTest scissorTest)
{
    FN("OGLRenderContext::ApplyScissorTest");

    if (scissorTest.rectangle_right - scissorTest.rectangle_left < 0)
    {
        LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Width must be greater than or equal to zero");
        return;
    }

    if (scissorTest.rectangle_bottom - scissorTest.rectangle_top < 0)
    {
        LOG_ASSERT(false, "renderState.ScissorTest.Rectangle.Height must be greater than or equal to zero");
        return;
    }

    if (m_renderState.scissorTest.Enabled != scissorTest.Enabled)
    {
        Enable(GL_SCISSOR_TEST, scissorTest.Enabled);
        m_renderState.scissorTest.Enabled = scissorTest.Enabled;
    }

    if (scissorTest.Enabled)
    {
        if (!(m_renderState.scissorTest.rectangle_bottom == scissorTest.rectangle_bottom && m_renderState.scissorTest.rectangle_left == scissorTest.rectangle_left && m_renderState.scissorTest.rectangle_right == scissorTest.rectangle_right &&
              m_renderState.scissorTest.rectangle_top == scissorTest.rectangle_top))
        {
            LOG_TRACE("glScissor");
            glScissor(scissorTest.rectangle_left, scissorTest.rectangle_bottom, scissorTest.rectangle_right - scissorTest.rectangle_left, scissorTest.rectangle_bottom - scissorTest.rectangle_top);
            m_renderState.scissorTest.rectangle_left = scissorTest.rectangle_left;
            m_renderState.scissorTest.rectangle_top = scissorTest.rectangle_top;
            m_renderState.scissorTest.rectangle_right = scissorTest.rectangle_right;
            m_renderState.scissorTest.rectangle_bottom = scissorTest.rectangle_bottom;
        }
    }
}

void OGLRenderContext::ApplyStencilTest(StencilTest stencilTest)
{
    FN("OGLRenderContext::ApplyStencilTest");

    if (m_renderState.stencilTest.Enabled != stencilTest.Enabled)
    {
        Enable(GL_STENCIL_TEST, stencilTest.Enabled);
        m_renderState.stencilTest.Enabled = stencilTest.Enabled;
    }

    if (stencilTest.Enabled)
    {
        ApplyStencil(StencilFace::Front, m_renderState.stencilTest.FrontFace, stencilTest.FrontFace);
        ApplyStencil(StencilFace::Back, m_renderState.stencilTest.BackFace, stencilTest.BackFace);
    }
}

void OGLRenderContext::ApplyStencil(StencilFace face, StencilTestFace currentTest, StencilTestFace test)
{
    FN("OGLRenderContext::ApplyStencil");

    if ((currentTest.StencilFailOperation != test.StencilFailOperation) || (currentTest.DepthFailStencilPassOperation != test.DepthFailStencilPassOperation) || (currentTest.DepthPassStencilPassOperation != test.DepthPassStencilPassOperation))
    {
        LOG_TRACE("glStencilOpSeparate");
        glStencilOpSeparate((GLenum)face, OGLTypeConverter::To(test.StencilFailOperation), OGLTypeConverter::To(test.DepthFailStencilPassOperation), OGLTypeConverter::To(test.DepthPassStencilPassOperation));

        currentTest.StencilFailOperation = test.StencilFailOperation;
        currentTest.DepthFailStencilPassOperation = test.DepthFailStencilPassOperation;
        currentTest.DepthPassStencilPassOperation = test.DepthPassStencilPassOperation;
    }

    if ((currentTest.Function != test.Function) || (currentTest.ReferenceValue != test.ReferenceValue) || (currentTest.CompareMask != test.CompareMask))
    {
        LOG_TRACE("glStencilFuncSeparate");
        glStencilFuncSeparate((GLenum)face, OGLTypeConverter::To(test.Function), test.ReferenceValue, test.CompareMask);

        currentTest.Function = test.Function;
        currentTest.ReferenceValue = test.ReferenceValue;
        currentTest.CompareMask = test.CompareMask;
    }

    if (currentTest.WriteMask != test.WriteMask)
    {
        LOG_TRACE("glStencilMaskSeparate");
        glStencilMaskSeparate((GLenum)face, test.WriteMask);
        currentTest.WriteMask = test.WriteMask;
    }
}

void OGLRenderContext::ApplyDepthTest(DepthTest depthTest)
{
    FN("OGLRenderContext::ApplyDepthTest");

    if (m_renderState.depthTest.Enabled != depthTest.Enabled)
    {
        Enable(GL_DEPTH_TEST, depthTest.Enabled);
        m_renderState.depthTest.Enabled = depthTest.Enabled;
    }

    if (depthTest.Enabled)
    {
        if (m_renderState.depthTest.Function != depthTest.Function)
        {
            LOG_TRACE("glDepthFunc");
            glDepthFunc(OGLTypeConverter::To(depthTest.Function));
            m_renderState.depthTest.Function = depthTest.Function;
        }
    }
}

void OGLRenderContext::ApplyDepthRange(DepthRange depthRange)
{
    FN("OGLRenderContext::ApplyDepthRange");

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

    if ((m_renderState.depthRange.Near != depthRange.Near) || (m_renderState.depthRange.Far != depthRange.Far))
    {
        LOG_TRACE("glDepthRange");
        glDepthRange(depthRange.Near, depthRange.Far);

        m_renderState.depthRange.Near = depthRange.Near;
        m_renderState.depthRange.Far = depthRange.Far;
    }
}

void OGLRenderContext::ApplyBlending(Blending blending)
{
    FN("OGLRenderContext::ApplyBlending");

    if (m_renderState.blending.Enabled != blending.Enabled)
    {
        Enable(GL_BLEND, blending.Enabled);
        m_renderState.blending.Enabled = blending.Enabled;
    }

    if (blending.Enabled)
    {
        if ((m_renderState.blending.SourceRGBFactor != blending.SourceRGBFactor) || (m_renderState.blending.DestinationRGBFactor != blending.DestinationRGBFactor) || (m_renderState.blending.SourceAlphaFactor != blending.SourceAlphaFactor) ||
            (m_renderState.blending.DestinationAlphaFactor != blending.DestinationAlphaFactor))
        {
            LOG_TRACE("glBlendFuncSeparate");
            glBlendFuncSeparate(OGLTypeConverter::To(blending.SourceRGBFactor), OGLTypeConverter::To(blending.DestinationRGBFactor), OGLTypeConverter::To(blending.SourceAlphaFactor), OGLTypeConverter::To(blending.DestinationAlphaFactor));

            m_renderState.blending.SourceRGBFactor = blending.SourceRGBFactor;
            m_renderState.blending.DestinationRGBFactor = blending.DestinationRGBFactor;
            m_renderState.blending.SourceAlphaFactor = blending.SourceAlphaFactor;
            m_renderState.blending.DestinationAlphaFactor = blending.DestinationAlphaFactor;
        }

        if ((m_renderState.blending.RGBEquation != blending.RGBEquation) || (m_renderState.blending.AlphaEquation != blending.AlphaEquation))
        {
            LOG_TRACE("glBlendEquationSeparate");
            glBlendEquationSeparate(OGLTypeConverter::To(blending.RGBEquation), OGLTypeConverter::To(blending.AlphaEquation));

            m_renderState.blending.RGBEquation = blending.RGBEquation;
            m_renderState.blending.AlphaEquation = blending.AlphaEquation;
        }

        if (m_renderState.blending.color != blending.color)
        {
            LOG_TRACE("glBlendColor");
            glBlendColor(blending.color[0], blending.color[1], blending.color[2], blending.color[3]);
            memcpy(&m_renderState.blending.color, &blending.color, sizeof(float) * 4);
        }
    }
}

void OGLRenderContext::ApplyColorMask(ColorMask colorMask)
{
    FN("OGLRenderContext::ApplyColorMask");

    if (m_renderState.colorMask != colorMask)
    {
        LOG_TRACE("glColorMask");
        glColorMask(colorMask.red, colorMask.green, colorMask.blue, colorMask.alpha);
        m_renderState.colorMask = colorMask;
    }
}

void OGLRenderContext::ApplyDepthMask(bool depthMask)
{
    FN("OGLRenderContext::ApplyDepthMask");

    if (m_renderState.depthMask != depthMask)
    {
        LOG_TRACE("glDepthMask");
        glDepthMask(depthMask);
        m_renderState.depthMask = depthMask;
    }
}
} // namespace bow
