#pragma once

namespace Bow {
	namespace Renderer{

		struct PrimitiveRestart
		{
		public:
			PrimitiveRestart()
			{
				Enabled = false;
				Index = 0;
			}

			bool Enabled;
			int Index;
		};

	}
}