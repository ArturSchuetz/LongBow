#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowTriangle.h"

#include "IObject.h"
#include "Triangle.h"
/**
* Ein Mesh
*/
class MeshObject : public IObject
{
public:
	MeshObject(std::vector<TriangleObject> triangles, int matIndex); // Erwartet Eckpunkte und Material

	virtual bool closestIntersection(Intersection& hit); // Am naechsten gelegener Schnittpunkt
	virtual bool anyIntersection(Bow::Core::Ray<float>& ray, float maxLambda); // Schnittpunkttest mit einem Strahl
	virtual void calcNormal(Intersection& hit); // Normale am Schnittpunkt berechnen

protected:
	std::vector<TriangleObject> m_Triangles;
};