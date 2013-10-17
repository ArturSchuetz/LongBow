#pragma once
#include "LongBow.h"
#include "IBowVertexAttribute.h"

namespace Bow {

	struct VertexAttributeFloatVec3 : VertexAttribute<float[3]>
    {
	public:
		VertexAttributeFloatVec3(std::string name);
        VertexAttributeFloatVec3(std::string name, int capacity);
    };

}