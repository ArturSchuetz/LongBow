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
Material::Material(const Bow::Core::ColorRGB& color, float diffuse, float specular, float phong)
{
	m_Color = color;
	m_Diffuse = diffuse;
	m_Specular = specular;
	m_PhongExponent = phong;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Beleuchtet den aktuellen Schnittpunkt.
* \param hit Schnittpunktinformationen
* \param light Lichtquelle
*/
Bow::Core::ColorRGB Material::shade(Intersection& hit, Lightsource& light)
{
	// ToDo: Implement
	float attenuation = 1.0 / (hit.getLocation() - light.getLocation()).Length();
	Bow::Core::ColorRGB LigntColor = attenuation * light.getColor();
	Bow::Core::ColorRGB Diffuse;
	Bow::Core::ColorRGB Specular;

	Bow::Core::Vector3<float> lightDir(light.getLocation() - hit.getLocation());
	lightDir.Normalize();

	float NdotL = DotP(hit.getNormal(), lightDir);
	if (NdotL > 0.0f)
	{
		Diffuse = LigntColor * getDiffuseReflectionColor() * NdotL;
		
		Bow::Core::Vector3<float> reflect(-lightDir - 2.0f * DotP(-lightDir, hit.getNormal()) * hit.getNormal());
		reflect.Normalize();

		Bow::Core::Vector3<float> ViewDir(hit.getRay().Origin - hit.getLocation());
		ViewDir.Normalize();

		float NdotV = DotP(reflect, ViewDir);
		if (NdotV > 0.0f)
		{
			Specular = LigntColor * powf(NdotV, getPhongExponent()); // Reflektionsfarbe = Lichtfarbe
			//Specular = LigntColor * m_Color * powf(NdotV, getPhongExponent()); // Color als Reflektionsfarbe
		}
	}
	return Diffuse + Specular;
}


float Material::getDiffuseReflection() const // Diffuser Reflexionsgrad
{
	return m_Diffuse;
}

Bow::Core::ColorRGB Material::getDiffuseReflectionColor() const	// Diffuser Reflexionsgrad als Farbe
{
	return m_Diffuse * m_Color;
}

float Material::getSpecularReflection() const // Spekularer Reflexionsgrad
{
	return m_Specular;
}

float Material::getPhongExponent() const // Phong Exponent
{
	return m_PhongExponent;
}

