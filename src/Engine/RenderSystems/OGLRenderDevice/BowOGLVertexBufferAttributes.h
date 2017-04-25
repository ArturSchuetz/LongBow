#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer {

		struct OGLVertexBufferAttribute
		{
		public:
			VertexBufferAttributePtr VertexBufferAttribute;
			bool Dirty;
		};

		typedef std::unordered_map<unsigned int, OGLVertexBufferAttribute> OGLVertexBufferAttributeMap;

		class OGLVertexBufferAttributes
		{
		public:
			OGLVertexBufferAttributes() : m_count(0), m_maximumArrayIndex(0), m_dirty(false) {}
			VertexBufferAttributeMap GetAttributes();

			VertexBufferAttributePtr GetAttribute(int index);
			void SetAttribute(int Location, VertexBufferAttributePtr attribute);

			void Clean();
			int GetMaximumArrayIndex();

		private:
			void Attach(int index);
			static void Detach(int index);

			static inline int NumberOfVertices(VertexBufferAttributePtr attribute);

		private:
			OGLVertexBufferAttributeMap m_Attributes;
			int							m_count;
			int							m_maximumArrayIndex;
			bool						m_dirty;
		};

	}
}