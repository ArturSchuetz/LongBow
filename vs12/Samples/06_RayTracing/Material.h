#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowMath.h"

class Intersection;
class Lightsource;

/**
* Klasse zur Sicherung von Materialien.
*/
class Material
{
public:
	Material(const Bow::Core::ColorRGB& color, float diffuse, float specular, float phong);

	Bow::Core::ColorRGB shade(Intersection& hit, Lightsource& light); // Farbe fuer einen Pixel

	float getDiffuseReflection() const;					// Diffuser Reflexionsgrad
	Bow::Core::ColorRGB getDiffuseReflectionColor() const;	// Diffuser Reflexionsgrad als Farbe
	float getSpecularReflection() const;					// Spekularer Reflexionsgrad
	float getPhongExponent() const;						// Phong Exponent

protected:
	Bow::Core::ColorRGB m_Color;	// Materialfarbe
	float m_Diffuse;				// Diffuser Reflexionsgrad
	float m_Specular;				// Spekularer Reflexionsgrad
	float m_PhongExponent;			// Phong Exponent
};
