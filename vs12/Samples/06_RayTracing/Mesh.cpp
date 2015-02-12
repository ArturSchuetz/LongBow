#include "Mesh.h"

#include "BowMath.h"

#include "Intersection.h"

#define EPSILON 0.001f

/**
* Konstruktor erwartet drei Eckpunkte und ein Material
* \param p0 Erster Punkt (Stuetzvektor)
* \param p1 Zweiter Punkt
* \param p2 Dritter Punkt
*/
MeshObject::MeshObject(std::vector<TriangleObject> triangles, int matIndex)
	: IObject(matIndex), m_Triangles(triangles)
{

}

/**
* Schnittpunkt, der am naechsten zum in Intersection uebergebenen Strahl liegt.
* \param hit Schnittpunktobjekt
* \return Erfolg
* \see Intersection
*/
bool MeshObject::closestIntersection(Intersection& hit)
{
	return false;
}

/**
* Testet ob mit einem Strahl geschnitten wird.
* Das Lambda des Schnittpunktes (abs) muss dabei unterhalb maxLambda liegen.
* \param ray Strahl
* \param maxLambda Maximale Lambda
* \return Erfolg
*/
bool MeshObject::anyIntersection(Bow::Core::Ray<float>& ray, float maxLambda)
{
	return false;
}

/**
* Normale am Schnittpunkt berechnen und im Schnittpunktobjekt sichern.
* \param hit Schnittpunktobjekt
* \see Intersection
*/
void MeshObject::calcNormal(Intersection& hit)
{

}

