#pragma once
#include "LongBow.h"
#include "IBowVertexAttribute.h"

namespace Bow {

	struct VertexAttributeFloat : VertexAttribute<float>
    {
	public:
		VertexAttributeFloat(std::string name);
        VertexAttributeFloat(std::string name, int capacity);
    };

}