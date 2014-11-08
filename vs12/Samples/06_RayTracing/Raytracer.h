#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "Intersection.h"
#include "IObject.h"

#define SQR(x) ((x) * (x))

class Scene; // forward declaration wg. Kreisverknuepfung

class Raytracer
{
public:
	Raytracer(); // Basiskonstruktor
	~Raytracer();

	void	init(int maxDepth, Scene* scene, unsigned char *pixels);
	void	renderLine(int lineNum);
	int		writePPM(Scene* scene, char* filename, unsigned char* pixels);

private:
	unsigned char *mPixels;
	int mPixelIndex;

	int mMaxDepth; // Maximale Reflektionsverfolgungstiefe
	Scene* mScene; // Zeiger auf Szene

	Bow::Core::Vector3<float>	trace(const Bow::Core::Ray<float>& ray, int depth);	// Strahlverfolgung
	Bow::Core::ColorRGB			shade(Intersection& hit, int depth); // Beleuchtung an Schnittpunkt
	void						setColor(int x, int y, const Bow::Core::ColorRGB& color);
};
