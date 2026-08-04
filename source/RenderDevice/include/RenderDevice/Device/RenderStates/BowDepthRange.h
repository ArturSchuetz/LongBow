#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

struct DepthRange
{
    DepthRange()
    {
        Near = 0.0;
        Far = 1.0;
    }

    float Near;
    float Far;

    bool operator==(const DepthRange &other) const
	{
		return Near == other.Near && Far == other.Far;
	}
};

} // namespace bow

namespace std
{

template <> struct hash<bow::DepthRange>
{
	size_t operator()(const bow::DepthRange &state) const
	{
		size_t hash = std::hash<float>()(state.Near);
		hash ^= std::hash<float>()(state.Far) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std