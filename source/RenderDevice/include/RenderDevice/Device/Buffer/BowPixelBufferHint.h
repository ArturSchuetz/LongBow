#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class PixelBufferHint : char
{
    Stream,
    Static,
    Dynamic
};

}
