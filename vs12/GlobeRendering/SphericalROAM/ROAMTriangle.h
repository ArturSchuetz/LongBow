#pragma once
#include "BowMath.h"

class ROAMTriangle
{
public:
	ROAMTriangle();
	ROAMTriangle(const bow::Vector3<float> &n1, const bow::Vector3<float> &n2, const bow::Vector3<float> &n3);

	void SetVertices(const bow::Vector3<float> &n1, const bow::Vector3<float> &n2, const bow::Vector3<float> &n3);

	// Triangle members
	bow::Vector3<float>	m_Vertex[3]; // Indices into CVertex::Array

	ROAMTriangle				*m_LeftChild;
	ROAMTriangle				*m_RightChild;
	ROAMTriangle				*m_BaseNeighbor;
	ROAMTriangle				*m_LeftNeighbor;
	ROAMTriangle				*m_RightNeighbor;
};