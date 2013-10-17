#pragma once
#include "LongBow.h"

namespace Bow {

enum class CullFace : char
{
    Front,
    Back,
    FrontAndBack
};

enum class WindingOrder : char
{
    Clockwise,
    Counterclockwise
};

class FacetCulling
{
public:
	FacetCulling()
    {
        Enabled = true;
        Face = CullFace::Back;
        FrontFaceWindingOrder = WindingOrder::Counterclockwise;
    }

    bool Enabled;
    CullFace Face;
    WindingOrder FrontFaceWindingOrder;
};

}