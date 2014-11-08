#include "Raytracer.h"
#include "Scene.h"

#include "BowMath.h"

#include <stdio.h>

/**
* Der Konstruktor nimmt einige Grundeinstellungen fuer die Strahlverfolgung und die Szene vor.
*/
Raytracer::Raytracer()
{
	mScene = NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Destruktor
*/
Raytracer::~Raytracer()
{
}

void Raytracer::init(int maxDepth, Scene* scene, unsigned char *pixels)
{
	mMaxDepth = maxDepth;
	mScene = scene;
	mPixels = pixels;
}

void Raytracer::renderLine(int lineNum)
{
	// eine Zeile berechnen
	for (unsigned int x = 0; x < mScene->getCamera()->GetXResolution(); x++) {
		setColor(x, lineNum, trace(mScene->getCamera()->Transform2Dto3D(x, lineNum), 0));
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Verfolgt einen Strahl und liefert die gefundene Farbe an die Ausgabefunktion.
* \param ray Kamerastrahl
* \param depth Aktuelle Reflektionstiefe
* \return Gefundene Farbe
*/
Bow::Core::Vector3<float> Raytracer::trace(const Bow::Core::Ray<float>& ray, int depth)
{
	Intersection hit(ray);

	if (mScene->getObjects()->closestIntersection(hit))
		return shade(hit, depth);

	return *(mScene->getBackgroundColor());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Hauptfunktion des Raytracers - ermittelt die Farbe (Leuchtdichte) an einem Schnittpunkt.
* Ist das gefundene Material spiegelnd, so wird trace() erneut mit depth+1 ausgefuehrt.
* \param hit Schnittpunkt
* \param depth Reflektionstiefe
* \return Gefundene Farbe
*/
Bow::Core::ColorRGB Raytracer::shade(Intersection& hit, int depth)
{
	// Normale im Schnittpunkt berechnen
	hit.calcNormal();

	// Material ermitteln
	Material* material = mScene->getMaterials()[hit.getMaterialIndex()];

	// Direkte Beleuchtung und Schattenwurf ...................

	Bow::Core::Vector3<float> color(0, 0, 0);
	for (LightList::iterator light = mScene->getLights().begin(); light != mScene->getLights().end(); light++)
	{
		Bow::Core::Ray<float> lightToHit((*light)->getLocation(), hit.getLocation() - (*light)->getLocation());

		if (DotP(lightToHit.Direction, hit.getNormal()) > 0) // Flaeche zeigt weg von der Lichtquelle
			continue;

		if (!mScene->getObjects()->anyIntersection(lightToHit, 1.0)) // Nichts dazwischen
			color = color + material->shade(hit, **light);
	}

	// Indirekte Beleuchtung (Spiegelung) ......................

	if ((material->getSpecularReflection()>0) && (depth<mMaxDepth))
	{
		Bow::Core::Vector3<float> direction = hit.getRay().Direction;
		direction.Normalize();

		Bow::Core::Vector3<float> incomingVector(direction);
		Bow::Core::Vector3<float> faceNormal(hit.getNormal());

		// Reflexionsvektor Berechnen
		Bow::Core::Ray<float> reflect(hit.getLocation() + 0.001f * faceNormal, incomingVector - 2.0f * DotP(incomingVector, faceNormal) * faceNormal);

		color = color + material->getSpecularReflection() * trace(reflect, depth + 1);
	}

	return color;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Farbe an der Stelle x,y eintragen
*/
void Raytracer::setColor(int x, int y, const Bow::Core::ColorRGB& color)
{
	int value;

	mPixelIndex = 3 * ((mScene->getCamera()->GetYResolution() - 1 - y) * mScene->getCamera()->GetXResolution() + x);
	// Rotkanal clampen und eintragen
	value = (int)(color[0] * 255);
	if (value < 0)
		value = 0;
	if (value > 255)
		value = 255;
	mPixels[mPixelIndex] = (unsigned char)(value);
	mPixelIndex++;

	// Gruenkanal clampen und eintragen
	value = (int)(color[1] * 255);
	if (value < 0)
		value = 0;
	if (value > 255)
		value = 255;
	mPixels[mPixelIndex] = (unsigned char)(value);
	mPixelIndex++;

	// Blaukanal clampen und eintragen
	value = (int)(color[2] * 255);
	if (value < 0)
		value = 0;
	if (value > 255)
		value = 255;
	mPixels[mPixelIndex] = (unsigned char)(value);
	mPixelIndex++;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Bild im ppm Format abspeichern
*/
int Raytracer::writePPM(Scene* scene, char* filename, unsigned char* pixels)
{
	FILE* fp = fopen(filename, "wb");

	if (!fp) {
		fprintf(stderr, "Error writing output file!\n");
		return -1;
	}

	fprintf(fp, "P6 %d %d 255\n", scene->getCamera()->GetXResolution(), scene->getCamera()->GetYResolution());

	for (int y = scene->getCamera()->GetYResolution() - 1; y >= 0; y--)
		fwrite(pixels + scene->getCamera()->GetXResolution()*y * 3, sizeof(unsigned char), scene->getCamera()->GetXResolution() * 3, fp);

	fclose(fp);
	return 0;
}
