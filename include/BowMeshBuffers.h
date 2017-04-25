#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class MeshBuffers
		{
		public:
			MeshBuffers() : m_vertexBufferAttributes(), IndexBuffer(nullptr){}
			~MeshBuffers(){}

			VertexBufferAttributeMap GetAttributes() { return m_vertexBufferAttributes; }
			void SetAttribute(unsigned int location, VertexBufferAttributePtr vertexBufferAttribute)
			{
				if (m_vertexBufferAttributes.find(location) == m_vertexBufferAttributes.end())
					m_vertexBufferAttributes[location] = vertexBufferAttribute;
				else
					m_vertexBufferAttributes.insert(std::pair<unsigned int, VertexBufferAttributePtr>(location, vertexBufferAttribute));
			}

			IndexBufferPtr IndexBuffer;

		private:
			VertexBufferAttributeMap m_vertexBufferAttributes;
		};

	}
}