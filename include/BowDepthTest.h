#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

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
		public:
			DepthTest()
			{
				Enabled = true;
				Function = DepthTestFunction::Less;
			}

			bool Enabled;
			DepthTestFunction Function;
		};

	}
}