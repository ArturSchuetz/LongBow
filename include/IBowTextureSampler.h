/**
 * @file IBowTextureSampler.h
 * @brief Declarations for IBowTextureSampler.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	enum class TextureMinificationFilter : char
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear,
	};

	enum class TextureMagnificationFilter : char
	{
		Nearest,
		Linear
	};

	enum class TextureWrap : char
	{
		Clamp,
		Repeat,
		MirroredRepeat
	};

	class ITextureSampler
	{
	protected:
		ITextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy) : MinificationFilter(minificationFilter), MagnificationFilter(magnificationFilter), WrapS(wrapS), WrapT(wrapT), MaximumAnistropy(maximumAnistropy) {}

	public:
		virtual ~ITextureSampler(){}

		const TextureMinificationFilter MinificationFilter;
		const TextureMagnificationFilter MagnificationFilter;
		const TextureWrap WrapS;
		const TextureWrap WrapT;
		const float MaximumAnistropy;
	};


}
