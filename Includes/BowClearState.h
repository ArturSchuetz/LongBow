#pragma once
#include "LongBow.h"
#include "BowScissorTest.h"
#include "BowColorMask.h"

namespace Bow {

typedef enum  TYPE_ClearBuffers
{
    ColorBuffer = 1,
    DepthBuffer = 2,
    StencilBuffer = 4,
    ColorAndDepthBuffer = ColorBuffer | DepthBuffer, 
    All = ColorBuffer | DepthBuffer | StencilBuffer
} ClearBuffers;

struct ClearState
{
public:
	ClearState() : ColorMask(true, true, true, true)
    {
        DepthMask			= true;
        FrontStencilMask	= ~0;
        BackStencilMask		= ~0;

        Buffers				= ClearBuffers::All;
		color[0]			= 1.0f;
		color[1]			= 1.0f;
		color[2]			= 1.0f;
		color[3]			= 1.0f;
        Depth				= 1;
        Stencil				= 0;
    }

    ScissorTest ScissorTest;
    ColorMask	ColorMask;
    bool		DepthMask;
    int			FrontStencilMask;
    int			BackStencilMask;
        
    ClearBuffers Buffers;
    float		 color[4];
    float		 Depth;
    int			 Stencil;
};

}