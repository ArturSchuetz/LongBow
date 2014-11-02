#pragma once
#include "BowSubMesh.h"

namespace Bow {
	namespace Core {

		SubMesh::SubMesh(Mesh* parent)
		{
			m_Parent = parent;
		}

		SubMesh::~SubMesh()
		{
		}
	}
}