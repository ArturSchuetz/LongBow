#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <RenderDevice/Device/RenderStates/BowBlending.h>
#include <RenderDevice/Device/RenderStates/BowColorMask.h>
#include <RenderDevice/Device/RenderStates/BowDepthRange.h>
#include <RenderDevice/Device/RenderStates/BowDepthTest.h>
#include <RenderDevice/Device/RenderStates/BowFaceCulling.h>
#include <RenderDevice/Device/RenderStates/BowPrimitiveRestart.h>
#include <RenderDevice/Device/RenderStates/BowScissorTest.h>
#include <RenderDevice/Device/RenderStates/BowStencilTest.h>
#include <RenderDevice/Device/RenderStates/BowStencilTestFace.h>

namespace bow
{

enum class ProgramPointSize : char
{
    Enabled,
    Disabled
};

enum class RasterizationMode : char
{
    Point,
    Line,
    Fill
};

struct RenderState
{
  public:
    RenderState() : colorMask(true, true, true, true)
    {
        programPointSize = ProgramPointSize::Disabled;
        rasterizationMode = RasterizationMode::Fill;
        depthMask = true;

        lineWidth = 1.0;
        pointSize = 1.0;
    }

    PrimitiveRestart primitiveRestart;
    FaceCulling faceCulling;
    ProgramPointSize programPointSize;
    RasterizationMode rasterizationMode;
    ScissorTest scissorTest;
    StencilTest stencilTest;
    DepthTest depthTest;
    DepthWrite depthWrite;
    DepthRange depthRange;
    Blending blending;
    ColorMask colorMask;
    bool depthMask;

    float lineWidth;
    float pointSize;

    bool operator==(const RenderState &other) const
    {
        // Hint: Viewport and scissor test are not compared because they are set dynamically in vulkan
        return primitiveRestart == other.primitiveRestart && faceCulling == other.faceCulling && programPointSize == other.programPointSize && rasterizationMode == other.rasterizationMode /* && scissorTest == other.scissorTest*/ &&
               stencilTest == other.stencilTest && depthTest == other.depthTest && depthWrite == other.depthWrite && depthRange == other.depthRange && blending == other.blending && colorMask == other.colorMask && depthMask == other.depthMask &&
               lineWidth == other.lineWidth && pointSize == other.pointSize;
    }
};

} // namespace bow

namespace std
{

template <> struct hash<bow::RenderState>
{
    size_t operator()(const bow::RenderState &state) const
    {
        size_t hash = std::hash<bow::PrimitiveRestart>()(state.primitiveRestart);
        hash ^= std::hash<bow::FaceCulling>()(state.faceCulling) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::ProgramPointSize>()(state.programPointSize) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::RasterizationMode>()(state.rasterizationMode) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        // hash ^= std::hash<bow::ScissorTest>()(state.scissorTest) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::StencilTest>()(state.stencilTest) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::DepthTest>()(state.depthTest) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::DepthWrite>()(state.depthWrite) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::DepthRange>()(state.depthRange) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::Blending>()(state.blending) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bow::ColorMask>()(state.colorMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bool>()(state.depthMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(state.lineWidth) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<float>()(state.pointSize) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

} // namespace std