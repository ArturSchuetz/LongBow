#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

struct ColorMask
{
    ColorMask(bool red, bool green, bool blue, bool alpha) : red(red), green(green), blue(blue), alpha(alpha) {}

    bool red;
    bool green;
    bool blue;
    bool alpha;

    bool operator==(const ColorMask &other) const { return red == other.red && green == other.green && blue == other.blue && alpha == other.alpha; }

    bool operator!=(const ColorMask &other) const { return !(*this == other); }
};

} // namespace bow

namespace std
{

template <> struct hash<bow::ColorMask>
{
    size_t operator()(const bow::ColorMask &mask) const
    {
        size_t hash = std::hash<bool>()(mask.red);
        hash ^= std::hash<bool>()(mask.green) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bool>()(mask.blue) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<bool>()(mask.alpha) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

} // namespace std