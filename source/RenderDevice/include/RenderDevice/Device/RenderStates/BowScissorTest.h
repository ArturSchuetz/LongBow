#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

struct ScissorTest
{
  public:
    ScissorTest()
    {
        Enabled = false;
        rectangle_top = 0;
        rectangle_left = 0;
        rectangle_bottom = 0;
        rectangle_right = 0;
    }

    bool Enabled;
    long rectangle_top;
    long rectangle_left;
    long rectangle_bottom;
    long rectangle_right;

    bool operator==(const ScissorTest &other) const
	{
		return (Enabled == other.Enabled &&
				rectangle_top == other.rectangle_top &&
				rectangle_left == other.rectangle_left &&
				rectangle_bottom == other.rectangle_bottom &&
				rectangle_right == other.rectangle_right);
	}
};

} // namespace bow

namespace std
{

template <> struct hash<bow::ScissorTest>
{
	size_t operator()(const bow::ScissorTest &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<long>()(state.rectangle_top) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<long>()(state.rectangle_left) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<long>()(state.rectangle_bottom) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<long>()(state.rectangle_right) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std