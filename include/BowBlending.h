#pragma once

namespace Bow {
	namespace Renderer{

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
		};

	}
}