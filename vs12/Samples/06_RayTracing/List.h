#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "Intersection.h"

typedef std::vector<IObject*> ObjectList;

/**
* Diese Klasse speichert Objekte in einer Liste.
* (Eine Alternative zur Liste ist die Bounding Volume Hierarchie).
* \see Object
*/

class List
{
public:
	virtual ~List(); // Destruktor loescht die Liste.

	bool closestIntersection(Intersection& hit); // Liefert den naheligensten Schnittpunkt
	bool anyIntersection(Bow::Core::Ray<float>& ray, float maxLambda); // Schnittpunkttest mit einem Strahl

	void addObject(IObject* newChild); // Fuegt ein Objekt der Liste hinzu

protected:
	ObjectList items; //! Vector mit den hinzugefuegten Objekten
};
