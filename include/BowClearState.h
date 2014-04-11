#pragma once
#include "BowPrerequisites.h"

#include "BowScissorTest.h"
#include "BowColorMask.h"

namespace Bow {
	namespace Renderer{

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
				Color[0] = 1.0f;
				Color[1] = 1.0f;
				Color[2] = 1.0f;
				Color[3] = 1.0f;
				Depth = 1;
				Stencil = 0;
			}

			ScissorTest ScissorTest;
			ColorMask	ColorMask;
			bool		DepthMask;

			ClearBuffers Buffers;
			float		 Color[4];
			float		 Depth;
			int			 Stencil;
		};

	}
}