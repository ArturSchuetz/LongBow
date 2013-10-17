#pragma once
#include "LongBow.h"

namespace Bow {

struct DepthRange
{
public:

	DepthRange()
    {
        Near = 0.0;
        Far = 1.0;
    }

    double Near;
    double Far;
};

}