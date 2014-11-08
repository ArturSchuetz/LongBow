#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

class Intersection;

/**
* Basisklasse fuer szenenrelevante Objekte jeglicher Form (Kugeln, Dreiecke).
* Enthaelt Funktionsprototypen fuer die Schnittpunktberechnung
*/
class IObject
{

public:
	// Konstruktor erwartet Material
	IObject(int matIndex) : m_MatIndex(matIndex) { }

	// Leerer Destruktor
	virtual ~IObject() { }

	virtual bool closestIntersection(Intersection& hit) = 0; // Prototyp
	virtual bool anyIntersection(Bow::Core::Ray<double>& ray, double maxLambda) = 0; // Prototyp

	virtual void calcNormal(Intersection& hit) = 0; // Prototyp2

	// Material des Objekts
	int getMaterialIndex() const 
	{
		return m_MatIndex;
	}

protected:
	int m_MatIndex; //! Material des Objekts
};
