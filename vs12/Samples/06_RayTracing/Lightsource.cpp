#include "Lightsource.h"
#include "BowMath.h"

//------------------------------------------------------------------------------

/**
* Konstruktor erwartet die Position, Farbe und Intensitaet der Lichtquelle.
* \param location Position
* \param color Farbe
* \param intensity Intensitaet
*/
Lightsource::Lightsource(const Bow::Core::Vector3<double>& location, const Bow::Core::ColorRGB& color, double intensity)
	:mLocation(location), mColor(color * intensity)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert die Position des Punktlichtes.
* \return Position
*/
Bow::Core::Vector3<double>& Lightsource::getLocation()
{
	return mLocation;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert die Farbe (Lichtstaerke) des Punktlichtes.
* \return Farbton
* \see Color
*/
Bow::Core::ColorRGB& Lightsource::getColor()
{
	return mColor;
}
