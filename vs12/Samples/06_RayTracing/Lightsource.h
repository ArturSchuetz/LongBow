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
	Lightsource(const Bow::Core::Vector3<float>& location, const Bow::Core::ColorRGB& color, float intensity); // Erwartet Position, Farbe und Intensitaet

	Bow::Core::Vector3<float>& getLocation(); // Position
	Bow::Core::ColorRGB& getColor(); // Farbe (Lichtstaerke)


protected:
	Bow::Core::Vector3<float> mLocation; //! Position
	Bow::Core::ColorRGB mColor; //! Farbe (Lichtstaerke)
};
