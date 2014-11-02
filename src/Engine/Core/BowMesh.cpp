#pragma once
#include "BowMesh.h"
#include "BowCore.h"

namespace Bow {
	namespace Core {

		Mesh::Mesh()
		{
			m_HasPositions = false;
			m_HasNormals = false;
			m_HasTextureCoords = false;
			m_HasTangents = false;

			m_NumberOfIndices = 0;
			m_NumberOfVertices = 0;

			m_Center[0] = m_Center[1] = m_Center[2] = 0.0f;
			m_Width = m_Height = m_Length = m_Radius = 0.0f;
		}

		Mesh::~Mesh()
		{
			delete[] Indices;

			delete[] Positions;
			delete[] Normals;
			delete[] Tangents;
			delete[] TextureCoords;
		}
	}
}