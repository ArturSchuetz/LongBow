#pragma once
#include "LongBow.h"

#include "BowBlending.h"
#include "BowColorMask.h"
#include "BowDepthRange.h"
#include "BowDepthTest.h"
#include "BowFacetCulling.h"
#include "BowPrimitiveRestart.h"
#include "BowScissorTest.h"
#include "BowStencilTest.h"
#include "BowStencilTestFace.h"

namespace Bow {

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

class RenderState
{
public:
	RenderState() : ColorMask(true, true, true, true)
    {
        ProgramPointSize = ProgramPointSize::Disabled;
        RasterizationMode = RasterizationMode::Fill;
        DepthMask = true;
    }

    PrimitiveRestart PrimitiveRestart;
    FacetCulling FacetCulling;
    ProgramPointSize ProgramPointSize;
    RasterizationMode RasterizationMode;
    ScissorTest ScissorTest;
    StencilTest StencilTest;
    DepthTest DepthTest;
    DepthRange DepthRange;
    Blending Blending;
    ColorMask ColorMask;
    bool DepthMask;
};

}