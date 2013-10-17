#pragma once
#include "LongBow.h"

namespace Bow {

enum class StencilOperation : char
{
	Zero,
    Invert,
    Keep,
    Replace,
    Increment,
    Decrement,
    IncrementWrap,
    DecrementWrap
};

enum class StencilTestFunction : char
{
    Never,
    Less,
    Equal,
    LessThanOrEqual,
    Greater,
    NotEqual,
    GreaterThanOrEqual,
    Always
};

class StencilTestFace
{
public:
	StencilTestFace()
    {
        StencilFailOperation = StencilOperation::Keep;
        DepthFailStencilPassOperation = StencilOperation::Keep;
        DepthPassStencilPassOperation = StencilOperation::Keep;
        Function = StencilTestFunction::Always;
        ReferenceValue = 0;
        Mask = ~0;
    }

    StencilOperation StencilFailOperation;
    StencilOperation DepthFailStencilPassOperation;
    StencilOperation DepthPassStencilPassOperation;
        
    StencilTestFunction Function;
    int ReferenceValue;
    int Mask;
};

}