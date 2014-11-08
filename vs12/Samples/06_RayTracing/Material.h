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
	Material(const Bow::Core::ColorRGB& color, double diffuse, double specular, double phong);

	Bow::Core::ColorRGB shade(Intersection& hit, Lightsource& light); // Farbe fuer einen Pixel

	double getDiffuseReflection() const;			// Diffuser Reflexionsgrad
	Bow::Core::ColorRGB getDiffuseReflectionColor() const;	// Diffuser Reflexionsgrad als Farbe
	double getSpecularReflection() const;		// Spekularer Reflexionsgrad
	double getPhongExponent() const;				// Phong Exponent

protected:
	Bow::Core::ColorRGB mColor;			// Materialfarbe
	double mDiffuse;			// Diffuser Reflexionsgrad
	double mSpecular;		// Spekularer Reflexionsgrad
	double mPhongExponent;	// Phong Exponent
};
