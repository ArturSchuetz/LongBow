#pragma once
#include "IBowVertexAttribute.h"

namespace Bow {
	namespace Core {

		struct VertexAttributeFloat : VertexAttribute<float>
		{
		public:
			VertexAttributeFloat(std::string name);
			VertexAttributeFloat(std::string name, int capacity);
		};

	}
}