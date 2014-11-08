#include "Triangle.h"

#include "BowMath.h"

#include "Intersection.h"

#define EPSILON 0.001f

/**
* Konstruktor erwartet drei Eckpunkte und ein Material
* \param p0 Erster Punkt (Stuetzvektor)
* \param p1 Zweiter Punkt
* \param p2 Dritter Punkt
*/
TriangleObject::TriangleObject(const Bow::Core::Triangle<float> &triangle, int matIndex)
	: IObject(matIndex)
{
	// Normale bestimmen und Ebene definieren
	m_PlaneBase = triangle.p0;
	m_PlaneVector1 = triangle.p1 - triangle.p0;
	m_PlaneVector2 = triangle.p2 - triangle.p0;
	m_Normal = Bow::Core::CrossP(m_PlaneVector1, m_PlaneVector2);
	m_Normal.Normalize();
}

/**
* Schnittpunkt, der am naechsten zum in Intersection uebergebenen Strahl liegt.
* \param hit Schnittpunktobjekt
* \return Erfolg
* \see Intersection
*/
bool TriangleObject::closestIntersection(Intersection& hit)
{
	Bow::Core::Ray<float> &ray = hit.getRay();
	Bow::Core::Vector3<float> direction = ray.Direction;
	Bow::Core::Vector3<float> origin(ray.Origin);

	// Testen ob Punkt im Dreieck (nach Moeller)
	// Umsetzung der Formel
	//	(t, u, v) = 1 / <p,v1> * (<q,v2>, <p,s>, <q,d>)
	//
	// wobei p = d x v2, q = s x v1

	Bow::Core::Vector3<float> p = Bow::Core::CrossP(direction, m_PlaneVector2);
	float qv1 = Bow::Core::DotP(m_PlaneVector1, p);

	// nenner~=0 ?
	if (qv1>-EPSILON && qv1<EPSILON)
		return false;

	float skalar = 1.0f / qv1;
	Bow::Core::Vector3<float> s = origin - m_PlaneBase;

	// u ermitteln es muss gelten 0 <= v <= 1
	float u = skalar*(DotP(s, p));

	if (u<0.0f || u>1.0f)
		return false;

	// v ermitteln
	Bow::Core::Vector3<float> q = Bow::Core::CrossP(s, m_PlaneVector1);
	float v = skalar*(Bow::Core::DotP(direction, q));

	if (v<0.0f || u + v>1.0f)
		return false;

	// t (lambda) ermitteln (ist bereits normalisiert)   
	float t = skalar*(Bow::Core::DotP(q, m_PlaneVector2));

	// Schnittpunkt "hinter" dem Ursprung de Strahles
	if (t<0)
		return false;

	hit.update(this, t);
	return true;
}

/**
* Testet ob mit einem Strahl geschnitten wird.
* Das Lambda des Schnittpunktes (abs) muss dabei unterhalb maxLambda liegen.
* \param ray Strahl
* \param maxLambda Maximale Lambda
* \return Erfolg
*/
bool TriangleObject::anyIntersection(Bow::Core::Ray<float>& ray, float maxLambda)
{
	Bow::Core::Vector3<float> direction = ray.Direction;
	Bow::Core::Vector3<float> origin(ray.Origin);

	Bow::Core::Vector3<float> p = Bow::Core::CrossP(direction, m_PlaneVector2);
	float qv1 = Bow::Core::DotP(m_PlaneVector1, p);
	if (qv1>-EPSILON && qv1<EPSILON)
		return false;

	float skalar = 1.0f / qv1;
	Bow::Core::Vector3<float> s = origin - m_PlaneBase;

	float u = skalar*(DotP(s, p));

	if (u<0.0f || u>1.0f)
		return false;

	Bow::Core::Vector3<float> q = Bow::Core::CrossP(s, m_PlaneVector1);
	float v = skalar*(Bow::Core::DotP(direction, q));

	if (v<0.0f || u + v>1.0f)
		return false;

	// t (lambda) ermitteln und normalisieren   
	float t = skalar*(Bow::Core::DotP(q, m_PlaneVector2));

	if (t<0)
		return false;

	return (t<maxLambda - EPSILON);
}

/**
* Normale am Schnittpunkt berechnen und im Schnittpunktobjekt sichern.
* \param hit Schnittpunktobjekt
* \see Intersection
*/
void TriangleObject::calcNormal(Intersection& hit)
{
	hit.getNormal() = m_Normal;
	hit.getNormal().Normalize();
}

