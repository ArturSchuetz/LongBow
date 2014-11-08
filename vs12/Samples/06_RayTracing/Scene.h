#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowCore.h"
#include "BowRenderer.h"

#include "Lightsource.h"
#include "Material.h"
#include "IObject.h"
#include "List.h"

#include <vector>

typedef std::vector<Lightsource*> LightList;
typedef std::vector<Material*> MaterialList;

class Scene
{
public:
	Scene(int width, int height);
	~Scene();

	Bow::Core::ColorRGB* getBackgroundColor(); // Liefert die Hintergrundfarbe
	Bow::Renderer::Camera* getCamera(); // Liefert die Kamera
	LightList& getLights(); // Liefert die Liste aller Lichter
	List* getObjects(); // Liefert die Objektliste
	MaterialList& getMaterials(); // Liefer die Liste aller Materialien der Szene

	int addMaterial(Material *newMaterial); // Fuegt ein Material der Szene hinzu
	void addObject(IObject *newObject); // Fuegt ein Objekt der Szene hinzu
	void addLight(Lightsource *newLight); // Fuegt ein Licht der Szene hinzu

private:
	Bow::Renderer::Camera* mCamera; //! Kameraobjekt (Viewport) der Szene
	Bow::Core::ColorRGB* mBackgroundColor; //! Hintergrundfarbe der Szene
	LightList mLights; //! Lichter der Szene
	MaterialList mMaterials; //! Verwendete Materialien
	List *mObjects; //! Objekte in der Szene

	// bool mFinished; //! Zustand der Objekthierarchie

};
