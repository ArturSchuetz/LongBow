#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class SourceBlendingFactor : char
{
    Zero,
    One,
    SourceAlpha,
    OneMinusSourceAlpha,
    DestinationAlpha,
    OneMinusDestinationAlpha,
    DestinationColor,
    OneMinusDestinationColor,
    SourceAlphaSaturate,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha
};

enum class DestinationBlendingFactor : char
{
    Zero,
    One,
    SourceColor,
    OneMinusSourceColor,
    SourceAlpha,
    OneMinusSourceAlpha,
    DestinationAlpha,
    OneMinusDestinationAlpha,
    DestinationColor,
    OneMinusDestinationColor,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha
};

enum class BlendEquation : char
{
    Add,
    Minimum,
    Maximum,
    Subtract,
    ReverseSubtract
};

struct Blending
{
  public:
    Blending()
    {
        Enabled = false;
        SourceRGBFactor = SourceBlendingFactor::One;
        SourceAlphaFactor = SourceBlendingFactor::One;
        DestinationRGBFactor = DestinationBlendingFactor::Zero;
        DestinationAlphaFactor = DestinationBlendingFactor::Zero;
        RGBEquation = BlendEquation::Add;
        AlphaEquation = BlendEquation::Add;
        color[0] = color[1] = color[2] = color[3] = 0.0f;
    }

    bool Enabled;
    SourceBlendingFactor SourceRGBFactor;
    SourceBlendingFactor SourceAlphaFactor;
    DestinationBlendingFactor DestinationRGBFactor;
    DestinationBlendingFactor DestinationAlphaFactor;
    BlendEquation RGBEquation;
    BlendEquation AlphaEquation;
    float color[4];

    bool operator==(const Blending &other) const
	{
		return Enabled == other.Enabled && SourceRGBFactor == other.SourceRGBFactor && SourceAlphaFactor == other.SourceAlphaFactor && DestinationRGBFactor == other.DestinationRGBFactor &&
			   DestinationAlphaFactor == other.DestinationAlphaFactor && RGBEquation == other.RGBEquation && AlphaEquation == other.AlphaEquation && color[0] == other.color[0] &&
			   color[1] == other.color[1] && color[2] == other.color[2] && color[3] == other.color[3];
	}
};

} // namespace bow

namespace std
{

template <> struct hash<bow::ProgramPointSize>
{
	size_t operator()(const bow::ProgramPointSize &state) const
	{
		return std::hash<int>()(static_cast<int>(state));
	}
};

template <> struct hash<bow::RasterizationMode>
{
	size_t operator()(const bow::RasterizationMode &state) const
	{
		return std::hash<int>()(static_cast<int>(state));
	}
};

template <> struct hash<bow::Blending>
{
	size_t operator()(const bow::Blending &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<int>()(static_cast<int>(state.SourceRGBFactor)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.SourceAlphaFactor)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.DestinationRGBFactor)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.DestinationAlphaFactor)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.RGBEquation)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.AlphaEquation)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<float>()(state.color[0]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<float>()(state.color[1]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<float>()(state.color[2]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<float>()(state.color[3]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std