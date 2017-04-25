#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowVertexAttribute.h"
#include "BowVector2.h"

namespace Bow {
	namespace Core {

		struct VertexAttributeFloat : VertexAttribute<float>
		{
		public:
			VertexAttributeFloat(const std::string& name);
			VertexAttributeFloat(const std::string& name, int capacity);
		};

	}
}