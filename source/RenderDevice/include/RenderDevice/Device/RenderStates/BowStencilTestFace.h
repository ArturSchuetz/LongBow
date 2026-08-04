#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class StencilOperation : char
{
    Zero,
    Invert,
    Keep,
    Replace,
    Increment,
    Decrement,
    IncrementWrap,
    DecrementWrap
};

enum class StencilTestFunction : char
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

struct StencilTestFace
{
  public:
    StencilTestFace()
    {
        StencilFailOperation = StencilOperation::Keep;
        DepthFailStencilPassOperation = StencilOperation::Keep;
        DepthPassStencilPassOperation = StencilOperation::Keep;
        Function = StencilTestFunction::Always;
        CompareMask = 0;
        WriteMask = 0;
        ReferenceValue = 0;
    }

    StencilOperation StencilFailOperation;
    StencilOperation DepthFailStencilPassOperation;
    StencilOperation DepthPassStencilPassOperation;

    StencilTestFunction Function;
    uint32_t CompareMask;
    uint32_t WriteMask;
    uint32_t ReferenceValue;

    bool operator==(const StencilTestFace &other) const
        {
        return (StencilFailOperation == other.StencilFailOperation) && (DepthFailStencilPassOperation == other.DepthFailStencilPassOperation) && (DepthPassStencilPassOperation == other.DepthPassStencilPassOperation) && (Function == other.Function) && (CompareMask == other.CompareMask) && (WriteMask == other.WriteMask) && (ReferenceValue == other.ReferenceValue);
		}

};

} // namespace bow

namespace std
{

template <> struct hash<bow::StencilTestFace>
{
	size_t operator()(const bow::StencilTestFace &state) const
	{
		size_t hash = std::hash<int>()(static_cast<int>(state.StencilFailOperation));
		hash ^= std::hash<int>()(static_cast<int>(state.DepthFailStencilPassOperation)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.DepthPassStencilPassOperation)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.Function)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>()(state.CompareMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>()(state.WriteMask) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<uint32_t>()(state.ReferenceValue) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std