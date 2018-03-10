#include "BowSubMesh.h"

#include "BowResources.h"

namespace bow {
	
	SubMesh::SubMesh()
		: m_parent(0)
	{

	}

	SubMesh::~SubMesh()
	{

	}

	unsigned int SubMesh::GetStartIndex()
	{
		return m_startIndex;
	}

	unsigned int SubMesh::GetNumIndices()
	{
		return m_numIndices;
	}
}
