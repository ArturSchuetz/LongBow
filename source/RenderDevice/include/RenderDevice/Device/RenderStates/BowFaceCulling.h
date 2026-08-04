#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <RenderDevice/Device/IBowRenderContext.h>

namespace bow
{

enum class CullFace : char
{
    Front,
    Back,
    FrontAndBack
};

enum class WindingOrder : char
{
    Clockwise,
    Counterclockwise
};

struct FaceCulling
{
  public:
    FaceCulling()
    {
        Enabled = true;
        Face = CullFace::Back;
        FrontFaceWindingOrder = WindingOrder::Counterclockwise;
    }

    bool Enabled;
    CullFace Face;
    WindingOrder FrontFaceWindingOrder;

    bool operator==(const FaceCulling &other) const {
		return Enabled == other.Enabled && Face == other.Face && FrontFaceWindingOrder == other.FrontFaceWindingOrder;
	}};

} // namespace bow

namespace std
{

template <> struct hash<bow::FaceCulling>
{
	size_t operator()(const bow::FaceCulling &state) const
	{
		size_t hash = std::hash<bool>()(state.Enabled);
		hash ^= std::hash<int>()(static_cast<int>(state.Face)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		hash ^= std::hash<int>()(static_cast<int>(state.FrontFaceWindingOrder)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		return hash;
	}
};

} // namespace std