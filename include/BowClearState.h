#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowScissorTest.h"
#include "BowColorMask.h"

#include "BowMath.h"

namespace bow {

	typedef enum TYPE_ClearBuffers
	{
		ColorBuffer = 1,
		DepthBuffer = 2,
		StencilBuffer = 4,
		ColorAndDepthBuffer = ColorBuffer | DepthBuffer,
		All = ColorBuffer | DepthBuffer | StencilBuffer
	} ClearBuffers;

	struct ClearState
	{
	public:
		ClearState() : ColorMask(true, true, true, true)
		{
			DepthMask = true;

			Buffers = ClearBuffers::All;
			Color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
			Depth = 1;
			Stencil = 0;
		}

		ScissorTest ScissorTest;
		ColorMask	ColorMask;
		bool		DepthMask;

		ClearBuffers	Buffers;
		ColorRGBA	Color;
		float			Depth;
		int				Stencil;
	};

}