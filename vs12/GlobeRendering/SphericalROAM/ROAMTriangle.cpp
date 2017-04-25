#include "ROAMTriangle.h"

ROAMTriangle::ROAMTriangle() 
{ 
	memset((char *)this, 0, sizeof(ROAMTriangle)); 
}

ROAMTriangle::ROAMTriangle(const Bow::Core::Vector3<float> &n1, const Bow::Core::Vector3<float> &n2, const Bow::Core::Vector3<float> &n3)
{
	memset((char *)this, 0, sizeof(ROAMTriangle));
	SetVertices(n1, n2, n3);
}

void ROAMTriangle::SetVertices(const Bow::Core::Vector3<float> &n1, const Bow::Core::Vector3<float> &n2, const Bow::Core::Vector3<float> &n3)
{
	m_Vertex[0] = n1; m_Vertex[1] = n2; m_Vertex[2] = n3;
}
