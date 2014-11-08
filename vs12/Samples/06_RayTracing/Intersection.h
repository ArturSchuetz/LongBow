#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowMath.h"

#include "IObject.h"

/**
* Klasse fuer die Behandlung von Schnittpunkten.
* Enthaelt unter anderem den Strahl und einen Zeiger auf das getroffene Objekt.
*/
class Intersection
{
public:

	Intersection(const Bow::Core::Ray<double>& ray); // Konstruktor erwartet eine Strahl

	Bow::Core::Ray<double>&  getRay(); // Auf den Strahl zugreifen
	double      getLambda() const; // Lambda des Schnittpunktes
	IObject* getObject() const; // Geschnittenes Objekt
	Bow::Core::Vector3<double>&     getLocation(); // Schnittpunkt
	Bow::Core::Vector3<double>&    getNormal(); // Normale am Schnittpunkt
	int        getMaterialIndex() const; // Material am Schnittpunkt

	void update(IObject* object, double lambda); // Schnittpunktinformationen setzen
	void calcNormal(); // Normale berechnen


protected:
	Bow::Core::Ray<double>		mRay; //! Strahl
	double						mLambda; //! Lambda
	IObject*					mObject; //! Geschnittenes Objekt
	Bow::Core::Vector3<double>	mLocation; //! Schnittpunkt
	Bow::Core::Vector3<double>	mNormal; //! Normale am Schnittpunkt
};
