#include <cfloat>
#include "Intersection.h"

//------------------------------------------------------------------------------

/**
* Konstruktor erwartet einen Strahl.
* Der Strahl wird Kopiert
* \param ray Strahl fuer Schnittpunktberechnung
*/
Intersection::Intersection(const Bow::Core::Ray<double>& ray)
	:mRay(ray)
{
	mLambda = FLT_MAX;
	mObject = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Auf den Strahl zugreifen
* \return Referenz auf den Strahl
*/
Bow::Core::Ray<double>& Intersection::getRay()
{
	return mRay;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Lambda des Schnittpunktes holen.
* \param Lambda (readonly)
*/
double Intersection::getLambda() const
{
	return mLambda;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Geschnittenes Objekt holen.
* \return Schnittobjekt (readonly)
*/
IObject* Intersection::getObject() const
{
	return mObject;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Koordinaten des Schnittpunktes holen.
* \return Schnittpunkt
*/
Bow::Core::Vector3<double>& Intersection::getLocation()
{
	return mLocation;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Normale am Schnittpunkt liefern.
* \return Normale an getLocation()
* \see getLocation()
*/
Bow::Core::Vector3<double>& Intersection::getNormal()
{
	return mNormal;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Material am Schnittpunkt liefern.
* \return Materialindex
* \see getLocation()
* \see getObject()
* \see Object->getMaterialIndex()
* \see Material
*/
int Intersection::getMaterialIndex() const
{
	return mObject->getMaterialIndex();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Daten des Schnittpunktes setzen.
* \param object Geschnittenes Objekt
* \param lambda Lambda des Schnittpunktes auf getRay()
* \see getRay()
*/
void Intersection::update(IObject* object, double lambda)
{
	mLambda = lambda;
	mObject = object;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Berechnet die Normale am Schnittpunkt.
* Sollte vor getNormal() aufgerufen werden.
* \see getNormal()
*/
void Intersection::calcNormal()
{
	mLocation = mRay.Origin + (mLambda *  mRay.Direction);
	mObject->calcNormal(*this);
}