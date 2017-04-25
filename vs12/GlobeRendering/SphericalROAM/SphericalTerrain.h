#pragma once
//
// Landscape Class
// Holds all the information to render an entire landscape.
//
#include <string>

class Bitmap;

class SphericalTerrain
{
public:
	SphericalTerrain();
	~SphericalTerrain();

	void Init(const std::string& Filename);

protected:
	Bitmap *m_HeightMap;
};