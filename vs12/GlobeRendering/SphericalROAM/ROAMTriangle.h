#pragma once
#include "BowMath.h"

class ROAMTriangle
{
public:
	ROAMTriangle();
	ROAMTriangle(const Bow::Core::Vector3<float> &n1, const Bow::Core::Vector3<float> &n2, const Bow::Core::Vector3<float> &n3);

	void SetVertices(const Bow::Core::Vector3<float> &n1, const Bow::Core::Vector3<float> &n2, const Bow::Core::Vector3<float> &n3);

	// Triangle members
	Bow::Core::Vector3<float>	m_Vertex[3]; // Indices into CVertex::Array

	ROAMTriangle				*m_LeftChild;
	ROAMTriangle				*m_RightChild;
	ROAMTriangle				*m_BaseNeighbor;
	ROAMTriangle				*m_LeftNeighbor;
	ROAMTriangle				*m_RightNeighbor;
};