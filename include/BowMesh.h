#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"
#include "IBowVertexAttribute.h"

namespace Bow {
	namespace Core {

		class Mesh
		{
			friend class ResourceManager;

		public:
			Mesh();
			~Mesh();

			IIndicesBase *Indices;
			Vector3<float> *Positions;
			Vector3<float> *Normals;
			Vector3<float> *Tangents;
			Vector2<float> *TextureCoords;

			bool HasIndices() const { return m_HasIndices; }
			bool HasPositions() const { return m_HasPositions; }
			bool HasNormals() const { return m_HasNormals; }
			bool HasTextureCoords() const { return m_HasTextureCoords; }
			bool HasTangents() const { return m_HasTangents; }

			unsigned int GetNumberOfIndices(){ return m_NumberOfIndices; }
			unsigned int GetNumberOfVertices(){ return m_NumberOfVertices; }

		private:
			bool m_HasIndices;
			bool m_HasPositions;
			bool m_HasNormals;
			bool m_HasTextureCoords;
			bool m_HasTangents;

			unsigned int m_NumberOfIndices;
			unsigned int m_NumberOfVertices;

			float m_Center[3];
			float m_Width;
			float m_Height;
			float m_Length;
			float m_Radius;
		};

	}
}