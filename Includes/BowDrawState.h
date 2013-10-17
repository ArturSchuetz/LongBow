#pragma once
#include "LongBow.h"

#include "BowRenderState.h"

namespace Bow {

struct H_DrawState
{
public:
    H_DrawState(RenderState renderState, ShaderProgramPtr shaderProgram, VertexArrayPtr vertexArray)
    {
        RenderState		= renderState;
        ShaderProgram	= shaderProgram;
        VertexArray		= vertexArray;
    }

    RenderState			RenderState;
    ShaderProgramPtr	ShaderProgram;
    VertexArrayPtr		VertexArray;
};

}