
#include "Scene.h"

/**
* Konstruktor legt die Defaultwerte einer Szene Fest.
* Im Detail sind das:
* - Eine Kamera bei 5,5,5 auf 0,0,0 mit 90 Grad fov
* - Hintergrundfarbe schwarz
* \param width Breite der Ausgabe
* \param height Hoehe der Ausgabe
*/
Scene::Scene(int width, int height)
{
	mObjects = new List();
	mCamera = new Bow::Renderer::Camera(Bow::Core::Vector3<float>(5, 5, 5), Bow::Core::Vector3<float>(0, 0, 0), Bow::Core::Vector3<float>(0, 1, 0), width, height);
	mBackgroundColor = new Bow::Core::ColorRGB(0.0, 0.0, 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Löscht alle dynamischen Szenenobjekte
*/
Scene::~Scene()
{
	delete mObjects;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert einen Zeiger auf die Hintergrundfarbe, ist also veränderbar.
* \return Hintergrundfarbe der Szene
*/
Bow::Core::ColorRGB* Scene::getBackgroundColor()
{
	return mBackgroundColor;
}

/**
* Liefert einen Zeiger auf die Szenenkamera, ist also veränderbar.
* \return Kameraobjekt
*/
Bow::Renderer::Camera* Scene::getCamera()
{
	return mCamera;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert eine Liste aller Lichter in der Szene (vector).
* \return Lichter der Szene
*/
LightList& Scene::getLights()
{
	return mLights;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert die aktuell verwendete Liste mit den enthaltenen Objekten.
* \return Objektliste der Szene
*/
List* Scene::getObjects()
{
	return mObjects;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Liefert eine Liste mit den Aktuell verfügbaren Materialien in der Szene.
* \return Materialien
*/
MaterialList& Scene::getMaterials()
{
	return mMaterials;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Fügt ein Material der Szene hinzu.
* \return Index des Materials
* \see Material
*/
int Scene::addMaterial(Material *newMaterial)
{
	mMaterials.push_back(newMaterial);
	return mMaterials.size() - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Fügt ein Objekt der Szenenhierachie hinzu.
* \see List
*/
void Scene::addObject(IObject *newObject)
{
	mObjects->addObject(newObject);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
* Fügt ein Licht der Szene hinzu.
*/
void Scene::addLight(Lightsource *newLight)
{
	mLights.push_back(newLight);
}

