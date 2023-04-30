#pragma once
#include <RLightTypes.h>
#include <vector>
using std::vector;
class RLights
{
	vector<RPointLight> pointLights;
	vector<RSpotLight> spotLights;
	vector<RDirectionalLight> directionalLights;
	vector<RAreaLight> areaLights;
};