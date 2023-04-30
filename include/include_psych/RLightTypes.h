/*
See the link below for variable meanings
https://docs.blender.org/manual/en/latest/render/lights/light_object.html
*/
#pragma once
#include <glm/common.hpp>
#include <glm/vec3.hpp>

class  RPointLight {
	glm::vec3 position;
	glm::vec3 color;
	float powerWatts;
	float clipRadius;
};
class RSpotLight {
	glm::vec3 position;
	glm::vec3 color;
	float powerWatts;
	float size; // angle 1 to 180
	float blend; // 0 to 1
};
class RAreaLight {
	// lmao not right now
};
class RDirectionalLight {
	glm::vec3 direction;
	glm::vec3 color;
	float powerWattsPerMtr2;
	float anglularDiameter;
};