#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

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
		};

	}
}