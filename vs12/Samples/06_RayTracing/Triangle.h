#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowTriangle.h"

#include "IObject.h"

/**
* Ein Dreieck (ein "Face").
*/
class TriangleObject : public IObject
{
public:
	TriangleObject(const Bow::Core::Triangle<double> &triangle, int matIndex); // Erwartet Eckpunkte und Material

	virtual bool closestIntersection(Intersection& hit); // Am naechsten gelegener Schnittpunkt
	virtual bool anyIntersection(Bow::Core::Ray<double>& ray, double maxLambda); // Schnittpunkttest mit einem Strahl
	virtual void calcNormal(Intersection& hit); // Normale am Schnittpunkt berechnen

protected:
	Bow::Core::Vector3<double> m_PlaneBase;
	Bow::Core::Vector3<double> m_PlaneVector1;
	Bow::Core::Vector3<double> m_PlaneVector2;
	Bow::Core::Vector3<double> m_Normal;
};