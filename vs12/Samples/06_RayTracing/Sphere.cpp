#include "Sphere.h"

#include "BowMath.h"
#include "Intersection.h"

#define SQR(x)  ((x) * (x))
#define ABS(x)	(((x)<0)?-(x):(x))

#define EPSILON     0.001f


//------------------------------------------------------------------------------

/**
* Konstruktor erwartet Zentrum, Radius und Material der Kugel.
* \param center Zentrum der Kugel
* \param radius Radius der Kugel
* \param matIndex Material der Kugel
* \see Material
*/
SphereObject::SphereObject(const Bow::Core::Sphere<float>& sphere, int matIndex)
	: IObject(matIndex), m_Sphere(sphere)
{
	mRadiusSqr = SQR(sphere.Radius);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Schnittpunkt, der am naechsten zum in Intersection uebergebenen Strahl liegt.
* \param hit Schnittpunktobjekt
* \return Erfolg
* \see Intersection
*/
bool SphereObject::closestIntersection(Intersection& hit)
{
	// Strahl ermitteln
	Bow::Core::Ray<float>& ray = hit.getRay();

	// Feststellen, ob der Strahl ausserhalb der Kugel beginnt
	Bow::Core::Vector3<float> originToCenter(m_Sphere.Center - ray.Origin);
	float lengthOcSqr = originToCenter.LengthSquared();
	bool  startsOutside = lengthOcSqr > mRadiusSqr + EPSILON;

	// Epsilonumgebung (Huelle) fuer Reflektion und Refraktion
	if (ABS(lengthOcSqr - mRadiusSqr) < EPSILON)
		return false;

	// 'Abstand' des Strahl-Ursprunges vom Lotfusspunkt berechnen
	Bow::Core::Vector3<float>& direction = ray.Direction;
	float dLen = direction.Length();
	float   distOF = DotP(originToCenter, direction) / direction.Length();

	// Strahl beginnt 'hinter' der Kugel ==> Kein Schnittpunkt
	if (startsOutside && distOF < EPSILON)
		return false;

	// 'Abstand' des Lotfusspunktes vom Schnittpunkt bestimmen
	float distFS = mRadiusSqr - (lengthOcSqr - SQR(distOF));

	// Strahl verfehlt die Kugel ==> Kein Schnittpunkt
	if (distFS < EPSILON)
		return false;

	// Geradenparameter des Schnittpunktes berechnen
	float lambda;
	if (startsOutside)
		lambda = (distOF - (float)sqrt(distFS)) / dLen;
	else
		lambda = (distOF + (float)sqrt(distFS)) / dLen;

	// Testen, ob der Schnittpunkt besser ist
	if (lambda > hit.getLambda())
		return false;

	hit.update(this, lambda);

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Testet ob mit einem Strahl geschnitten wird.
* Das Lambda des Schnittpunktes (abs) muss dabei unterhalb maxLambda liegen.
* \param ray Strahl
* \param maxLambda Maximale Lambda
* \return Erfolg
*/
bool SphereObject::anyIntersection(Bow::Core::Ray<float>& ray, float maxLambda)
{
	// Feststellen, ob der Strahl ausserhalb der Kugel beginnt
	Bow::Core::Vector3<float> originToCenter(m_Sphere.Center - ray.Origin);
	float lengthOcSqr = originToCenter.LengthSquared();
	bool  startsOutside = lengthOcSqr > mRadiusSqr + EPSILON;

	// 'Abstand' des Strahl-Ursprunges vom Lotfusspunkt berechnen
	Bow::Core::Vector3<float>& direction = ray.Direction;
	float dLen = direction.Length();
	float distOF = DotP(originToCenter, direction) / dLen;

	// Strahl beginnt 'hinter' der Kugel ==> Kein Schnittpunkt
	if (startsOutside && distOF < EPSILON)
		return false;

	// 'Abstand' des Lotfusspunktes vom Schnittpunkt bestimmen
	float distFS = mRadiusSqr - (lengthOcSqr - SQR(distOF));

	// Strahl verfehlt die Kugel ==> Kein Schnittpunkt
	if (distFS < EPSILON)
		return false;

	// Geradenparameter des Schnittpunktes berechnen
	float lambda;
	if (startsOutside)
		lambda = (distOF - (float)sqrt(distFS)) / dLen;
	else
		lambda = (distOF + (float)sqrt(distFS)) / dLen;

	// Um "Akne" bei der Schattierung zu verhindern Epsilonumgebung einsetzen
	return (lambda < maxLambda - EPSILON);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Normale am Schnittpunkt berechnen und im Schnittpunktobjekt sichern.
* \param hit Schnittpunktobjekt
* \see Intersection
*/
void SphereObject::calcNormal(Intersection& hit)
{
	hit.getNormal() = hit.getLocation() - m_Sphere.Center;
	hit.getNormal().Normalize();
}

