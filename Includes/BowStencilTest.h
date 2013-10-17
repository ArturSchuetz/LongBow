#pragma once
#include "LongBow.h"
#include "BowStencilTestFace.h"

namespace Bow {

class StencilTest
{
public:
	StencilTest()
    {
        Enabled = false;
    }

    bool Enabled;
    StencilTestFace FrontFace;
    StencilTestFace BackFace;
};

}