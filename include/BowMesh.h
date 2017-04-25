#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"
#include "IBowVertexAttribute.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		// ---------------------------------------------------------------------------
		/** @brief A mesh represents geometry without any material.
		*/
		class Mesh
		{
		public:
			VertexAttributePtr GetAttribute(std::string name) { return m_attributes.find(name)->second; };
			void AddAttribute(VertexAttributePtr vertexAttribute) {
				m_attributes.insert(std::pair<std::string, VertexAttributePtr>(vertexAttribute->Name, vertexAttribute));
			}

			IndicesBasePtr Indices;

			Mesh() : m_attributes(), Indices(nullptr) { }
			~Mesh() { }

		private:
			VertexAttributeMap m_attributes;
		};

	}
}