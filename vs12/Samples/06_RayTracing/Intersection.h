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

	Intersection(const Bow::Core::Ray<float>& ray); // Konstruktor erwartet eine Strahl

	Bow::Core::Ray<float>&			getRay(); // Auf den Strahl zugreifen
	float							getLambda() const; // Lambda des Schnittpunktes
	IObject*						getObject() const; // Geschnittenes Objekt
	Bow::Core::Vector3<float>&		getLocation(); // Schnittpunkt
	Bow::Core::Vector3<float>&		getNormal(); // Normale am Schnittpunkt
	int								getMaterialIndex() const; // Material am Schnittpunkt

	void update(IObject* object, float lambda); // Schnittpunktinformationen setzen
	void calcNormal(); // Normale berechnen


protected:
	Bow::Core::Ray<float>		mRay; //! Strahl
	float						mLambda; //! Lambda
	IObject*					mObject; //! Geschnittenes Objekt
	Bow::Core::Vector3<float>	mLocation; //! Schnittpunkt
	Bow::Core::Vector3<float>	mNormal; //! Normale am Schnittpunkt
};
