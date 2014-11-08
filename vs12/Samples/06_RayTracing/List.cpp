#include "List.h"


//------------------------------------------------------------------------------

/**
* Destruktor loescht alle Elemente aus der Liste.
*/
List::~List()
{
	for (ObjectList::iterator item = items.begin(); item != items.end(); item++)
		delete (*item);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert den Schnittpunkt, der am naechsten zum Ursprung des Strahls liegt.
* Es werden hierbei alle Elemente in der Liste getestet.
* \param hit Ein Intersectionobjekt, in dem der Schnittpunkt gespeichert wird
* \return Liefert zurueck ob das Schneiden erfolgreich war
* \see Intersection
*/
bool List::closestIntersection(Intersection & hit)
{
	bool result = false;
	Intersection hit2(hit.getRay());

	for (ObjectList::iterator item = items.begin(); item != items.end(); item++) {
		result |= (*item)->closestIntersection(hit2);
		if (hit2.getLambda() < hit.getLambda())
			hit = hit2;
	}

	return result;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Testet ob irgendein Schnittpunkt vorliegt.
* Es werden alle Elemente in er Liste durchlaufen.
* \param ray Strahl der getestet werden soll
* \param maxLambda Maximal erlaubte Entfehrnung des Schnittpunkts (rel. zur Laenge von ray)
* \return Liefert zurueck ob ein Schnittpunkt existiert
* \see Ray
*/
bool List::anyIntersection(Bow::Core::Ray<double>& ray, double maxLambda)
{
	ObjectList::iterator item = items.begin();
	while (item != items.end()) {
		if ((*item)->anyIntersection(ray, maxLambda))
			return true;

		item++;
	}

	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Fuegt ein Objekt der Liste hinzu.
* \param newChild Das hinzuzufuegende Objekt
*/
void List::addObject(IObject* newChild)
{
	items.push_back(newChild);
}


