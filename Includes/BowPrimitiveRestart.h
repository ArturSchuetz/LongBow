#pragma once
#include "LongBow.h"

namespace Bow {

class PrimitiveRestart
{
public:
	PrimitiveRestart()
    {
        Enabled = false;
        Index = 0;
    }

    bool Enabled;
    int Index;
};

}