#pragma once
#include <RLightTypes.h>
#include <RCameraObject.h>
#include <RObject.h>
#include <FileIO.h>
#include <unordered_map>
class RScene {
public:
	// all loaded
	vector<RObject*> Objects; // all objects loaded
	vector<RPointLight*> pLights; // all point lights loaded
	vector<RDirectionalLight*> dLights; // all directional lights loaded
	vector<RSpotLight*> sLights; // all spot lights loaded
	vector<RAreaLight*> aLights; // all area lights loaded

	RScene(std::string sceneName) {
		parseScene(getStringFromDisk("\\Scenes\\" + sceneName + ".scene"));
	}
	void parseScene(std::string data);
	RObject* getObject(string name);
	void LoadObjects();

};