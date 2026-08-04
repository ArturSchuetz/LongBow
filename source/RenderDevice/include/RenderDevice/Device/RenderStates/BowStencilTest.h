#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <RenderDevice/Device/RenderStates/BowStencilTestFace.h>

namespace bow
{

struct StencilTest
{
  public:
    StencilTest() { Enabled = false; }

    bool Enabled;
    StencilTestFace FrontFace;
    StencilTestFace BackFace;

    bool operator==(const StencilTest &other) const
	{
		return (Enabled == other.Enabled) && (FrontFace == other.FrontFace) && (BackFace == other.BackFace);
	}
};

} // namespace bow

namespace std
{

template <> struct hash<bow::StencilTest>
{
	size_t operator()(const bow::StencilTest &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<bow::StencilTestFace>()(state.FrontFace) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<bow::StencilTestFace>()(state.BackFace) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std