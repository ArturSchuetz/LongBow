#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <CoreSystems/BowCorePredeclares.h>
#include <CoreSystems/BowMath.h>

namespace bow
{

enum class ShaderStage : uint8_t
{
    Vertex,
    TessellationControl,
    TessellationEvaluation,
    Geometry,
    Fragment,
    Compute,
    RayGen,
    AnyHit,
    ClosestHit,
    Miss,
    Intersection,
    Callable
};

enum class PrimitiveType : uint8_t
{
    Points,
    Lines,
    LineLoop,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
    LinesAdjacency,
    LineStripAdjacency,
    TrianglesAdjacency,
    TriangleStripAdjacency
};

struct Viewport
{
    Viewport() : x(0), y(0), width(0), height(0) {}
    Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    int x;
    int y;
    int width;
    int height;

    bool operator==(const Viewport &vp) const { return (x == vp.x && y == vp.y && width == vp.width && height == vp.height); }
    bool operator!=(const Viewport &vp) const { return (x != vp.x || y != vp.y || width != vp.width || height != vp.height); }
};

class IRenderContext
{
  public:
    virtual ~IRenderContext() {}

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================
    virtual void VRelease() = 0;

    // =========================================================================
    // RENDERING STUFF:
    // =========================================================================
    virtual VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) = 0;
    virtual VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers) = 0;
    virtual VertexAttributeBindingsPtr VCreateVertexAttributeBindings() = 0;
    virtual FramebufferPtr VCreateFramebuffer() = 0;

    virtual void VBeginFrame() = 0;
    virtual void VEndFrame() = 0;

    virtual void VClear(ClearState clearState) = 0;
    virtual void VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) = 0;
    virtual void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) = 0;
    virtual void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState) = 0;
    virtual void VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end) = 0;

    virtual void VSetFramebuffer(FramebufferPtr framebufer) = 0;
    virtual void VSetViewport(Viewport viewport) = 0;
    virtual Viewport VGetViewport() = 0;

    virtual void VSwapBuffers(bool vsync = false) = 0;

    // ==================== RayTracing ====================

    virtual void VTraceRays(void *shaderProgram, ShaderResourceBindingsPtr resourceBindings, Texture2DPtr outputImage, uint32_t width, uint32_t height) = 0;
};

} // namespace bow

namespace std
{

template <> struct hash<bow::Viewport>
{
    size_t operator()(const bow::Viewport &vp) const
    {
        size_t hash = std::hash<int>()(vp.x);
        hash ^= std::hash<int>()(vp.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>()(vp.width) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>()(vp.height) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

} // namespace std