#pragma once

#ifdef _WIN32
	#include <Windows.h>
#endif

namespace Bow {
	namespace Renderer{

		struct ScissorTest
		{
		public:
			ScissorTest()
			{
				Enabled = false;
				rectangle.top = 0;
				rectangle.left = 0;
				rectangle.bottom = 0;
				rectangle.right = 0;
			}

			bool Enabled;
			RECT rectangle;
		};

	}
}