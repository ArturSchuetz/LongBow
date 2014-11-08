#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowSphere.h"
#include "IObject.h"

/**
* Eine "echte" Kugel, also keine Kugel aus Dreiecken.
*/
class SphereObject : public IObject
{
public:
	SphereObject(const Bow::Core::Sphere<double>& sphere, int matIndex); // Konstruktor erwartet Zentrum, Radius und Material

	virtual bool closestIntersection(Intersection& hit); // Am naechsten gelegener Schnittpunkt
	virtual bool anyIntersection(Bow::Core::Ray<double>& ray, double maxLambda); // Schnittpunkttest mit einem Strahl
	virtual void calcNormal(Intersection& hit); // Normale am Schnittpunkt berechnen

protected:
	Bow::Core::Sphere<double> m_Sphere; // Zentrum der Kugel
	double mRadiusSqr; // Radiusquadrat der Kugel
};