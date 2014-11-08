#include "Material.h"

#include <cmath>

#include "Intersection.h"
#include "Lightsource.h"

#ifndef PI
#define PI 3.141592
#endif

//------------------------------------------------------------------------------

/**
* Der Konstruktor fuer ein Material mit Phong BRDF.
* color: Materialfarbe
* diffuse: Diffuser Reflexionsgrad
* specular: Spekularer Reflexionsgrad
* phong: Phong Exponent (Glanzpunkt Grösse)
*/
Material::Material(const Bow::Core::ColorRGB& color, double diffuse, double specular, double phong)
{
	mColor = color;
	mDiffuse = diffuse;
	mSpecular = specular;
	mPhongExponent = phong;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Beleuchtet den aktuellen Schnittpunkt.
* \param hit Schnittpunktinformationen
* \param light Lichtquelle
*/
Bow::Core::ColorRGB Material::shade(Intersection& hit, Lightsource& light)
{
	return mColor;      // Alles schwarz im Moment...
}


double Material::getDiffuseReflection() const			// Diffuser Reflexionsgrad
{
	return mDiffuse;
}

Bow::Core::ColorRGB Material::getDiffuseReflectionColor() const	// Diffuser Reflexionsgrad als Farbe
{
	return mDiffuse * mColor;
}

double Material::getSpecularReflection() const		// Spekularer Reflexionsgrad
{
	return mSpecular;
}

double Material::getPhongExponent() const				// Phong Exponent
{
	return mPhongExponent;
}

