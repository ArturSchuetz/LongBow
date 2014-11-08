#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowMath.h"

/**
* Eine Lichtquelle (Punktlicht).
*/
class Lightsource
{
public:
	Lightsource(const Bow::Core::Vector3<double>& location, const Bow::Core::ColorRGB& color, double intensity); // Erwartet Position, Farbe und Intensitaet

	Bow::Core::Vector3<double>& getLocation(); // Position
	Bow::Core::ColorRGB& getColor(); // Farbe (Lichtstaerke)


protected:
	Bow::Core::Vector3<double> mLocation; //! Position
	Bow::Core::ColorRGB mColor; //! Farbe (Lichtstaerke)
};
