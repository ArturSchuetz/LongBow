#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class DepthTestFunction : char
{
    Never,
    Less,
    Equal,
    LessThanOrEqual,
    Greater,
    NotEqual,
    GreaterThanOrEqual,
    Always
};

struct DepthTest
{
    DepthTest()
    {
        Enabled = true;
        Function = DepthTestFunction::Less;
    }

    bool Enabled;
    DepthTestFunction Function;

    bool operator==(const DepthTest& other) const
	{
		return (Enabled == other.Enabled) && (Function == other.Function);
	}
};

struct DepthWrite
{
    DepthWrite()
    {
        Enabled = true;
    }

    bool Enabled;

    bool operator==(const DepthWrite &other) const { return (Enabled == other.Enabled); }
};

} // namespace bow

namespace std
{

template <> struct hash<bow::DepthTest>
{
	size_t operator()(const bow::DepthTest &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<int>()(static_cast<int>(state.Function)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

template <> struct hash<bow::DepthWrite>
{
    size_t operator()(const bow::DepthWrite &state) const
    {
        size_t hash = std::hash<bool>()(state.Enabled);
        return hash;
    }
};

} // namespace std