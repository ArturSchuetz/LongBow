#pragma once
#include "LongBow.h"

namespace Bow {

	enum class BufferHint : char
    {
        StreamDraw,
        StreamRead,
        StreamCopy,
        StaticDraw,
        StaticRead,
        StaticCopy,
        DynamicDraw,
        DynamicRead,
        DynamicCopy,
    };

}