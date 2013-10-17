#pragma once
#include "LongBow.h"

namespace Bow {

enum class DepthTestFunction : char
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

class DepthTest
{
public:
	DepthTest()
    {
        Enabled = true;
        Function = DepthTestFunction::Less;
    }

    bool Enabled;
    DepthTestFunction Function;
};

}