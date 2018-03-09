#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"
#include "IBowVertexAttribute.h"

#include "BowMath.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A mesh represents geometry without any material.
	*/
	class MeshAttribute
	{
	public:
		VertexAttributePtr GetAttribute(std::string name) { return m_attributes.find(name)->second; };
		void AddAttribute(VertexAttributePtr vertexAttribute) {
			m_attributes.insert(std::pair<std::string, VertexAttributePtr>(vertexAttribute->Name, vertexAttribute));
		}

		IndicesBasePtr Indices;

		MeshAttribute() : m_attributes(), Indices(nullptr) { }
		~MeshAttribute() { }

	private:
		VertexAttributeMap m_attributes;
	};
}
