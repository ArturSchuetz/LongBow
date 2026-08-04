#pragma once
#include <OpenGLRenderDevice/BowOGLRenderDevice.h>
#include <OpenGLRenderDevice/BowOGLRenderDevicePredeclares.h>
#include <OpenGLRenderDevice/Device/Context/FrameBuffer/BowOGLFramebuffer.h>
#include <OpenGLRenderDevice/Device/Shader/BowOGLShaderProgram.h>
#include <OpenGLRenderDevice/Device/Textures/BowOGLTextureUnits.h>
#include <OpenGLRenderDevice/OpenGLRenderDevice_api.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/BowGraphicsWindow.h>
#include <RenderDevice/Device/IBowRenderContext.h>

#include <CoreSystems/BowCorePredeclares.h>
#include <CoreSystems/BowMath.h>

struct GLFWwindow;

namespace bow
{

typedef std::shared_ptr<class OGLShaderProgram> OGLShaderProgramPtr;
typedef std::shared_ptr<class OGLTextureUnits> OGLTextureUnitsPtr;
typedef std::shared_ptr<class OGLFramebuffer> OGLFramebufferPtr;

enum class StencilFace : uint32_t;
enum class MaterialFace : uint32_t;

class OGLRenderContext : public IRenderContext
{
  public:
    OGLRenderContext(GLFWwindow *window);
    ~OGLRenderContext();

    // =========================================================================
    // Init/Release stuff

    bool Initialize(OGLRenderDevice *device);
    void VRelease() override;

    // =========================================================================
    // Inherited via IRenderContext

    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings() override;
    FramebufferPtr VCreateFramebuffer() override;

    void VBeginFrame() override;
    void VEndFrame() override;

    void VClear(struct ClearState clearState) override;
    void VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end);

    void VSetFramebuffer(FramebufferPtr framebufer) override;
    void VSetViewport(Viewport viewport) override;
    Viewport VGetViewport() override;

    void VSwapBuffers(bool vsync) override;

    // ==================== RayTracing ====================

    void VTraceRays(void *shaderProgram, ShaderResourceBindingsPtr resourceBindings, Texture2DPtr outputImage, uint32_t width, uint32_t height) override;

  private:
    // you shall not copy
    OGLRenderContext(OGLRenderContext &) = delete;
    OGLRenderContext &operator=(const OGLRenderContext &) = delete;

    void Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState);

    void ApplyVertexAttributeBindings(VertexAttributeBindingsPtr vertexAttributeBindings);
    void ApplyShaderResourceBindings(ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram);
    void ApplyShaderProgram(ShaderProgramPtr shaderProgram);
    void ApplyFramebuffer();

    void ForceApplyRenderState(RenderState renderState);
    void ForceApplyRenderStateStencil(StencilFace face, StencilTestFace test);

    void ApplyRenderState(RenderState renderState);
    void ApplyPrimitiveRestart(PrimitiveRestart primitiveRestart);
    void ApplyFaceCulling(FaceCulling FaceCulling);
    void ApplyProgramPointSize(ProgramPointSize programPointSize);
    void ApplyRasterizationMode(RasterizationMode rasterizationMode);
    void ApplyScissorTest(ScissorTest scissorTest);
    void ApplyStencilTest(StencilTest stencilTest);
    void ApplyStencil(StencilFace face, StencilTestFace currentTest, StencilTestFace test);
    void ApplyDepthTest(DepthTest depthTest);
    void ApplyDepthRange(DepthRange depthRange);
    void ApplyBlending(Blending blending);
    void ApplyColorMask(ColorMask colorMask);
    void ApplyDepthMask(bool depthMask);

    Viewport m_viewport;

    ColorRGBA m_clearColor;
    float m_clearDepth;
    int m_clearStencil;

    RenderState m_renderState;
    OGLShaderProgramPtr m_boundShaderProgram;
    OGLTextureUnitsPtr m_textureUnits;

    OGLFramebufferPtr m_boundFramebuffer;
    OGLFramebufferPtr m_setFramebuffer;

    GLFWwindow *m_window;
    OGLRenderDevice *m_device;

    int m_versionMajor;
    int m_versionMinor;
    bool m_hasDirectStateAccess;

    //! Creates the buffer, vertex array and shader VDrawLine needs, once.
    bool EnsureLineResources();

    uint32_t m_lineProgram;
    uint32_t m_lineVertexBuffer;
    uint32_t m_lineVertexArray;
    static OGLRenderContext *m_currentContext;
    bool m_initialized;
    bool m_vsync;
};

typedef std::shared_ptr<OGLRenderContext> OGLRenderContextPtr;

} // namespace bow
