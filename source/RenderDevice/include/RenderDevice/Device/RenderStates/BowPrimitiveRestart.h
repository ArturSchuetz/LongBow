#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

struct PrimitiveRestart
{
  public:
    PrimitiveRestart()
    {
        Enabled = false;
        Index = 0;
    }

    bool Enabled;
    int Index;

    bool operator==(const PrimitiveRestart &other) const
    {
        return Enabled == other.Enabled &&
			Index == other.Index;
    }
};

} // namespace bow

namespace std
{

template <> struct hash<bow::PrimitiveRestart>
{
	size_t operator()(const bow::PrimitiveRestart &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<int>()(state.Index) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std