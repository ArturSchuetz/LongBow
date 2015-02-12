#pragma once
#include "BowMesh.h"
#include "BowCore.h"

namespace Bow {
	namespace Core {

		Mesh::Mesh()
		{
			m_Center[0] = m_Center[1] = m_Center[2] = 0.0f;
			m_Width = m_Height = m_Length = m_Radius = 0.0f;

			m_Indices = nullptr;
		}

		Mesh::~Mesh()
		{
			delete[] m_Indices;
			m_SubMeshes.clear();
			m_Materials.clear();
			
			m_Positions.clear();
			m_Normals.clear();
			m_Tangents.clear();
		}
	}
}