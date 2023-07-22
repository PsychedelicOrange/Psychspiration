/*
See the link below for variable meanings
https://docs.blender.org/manual/en/latest/render/lights/light_object.html
*/
#pragma once
#include <glm/common.hpp>
#include <glm/vec3.hpp>

class  RPointLight {
public:
	glm::vec3 position = glm::vec3(1, 5, 1);
	glm::vec3 color = glm::vec3(1,1,1);
	float powerWatts = 2.1;
	float clipRadius = 0;
};

class RSpotLight {
public:
	glm::vec3 position = glm::vec3(1, 1, 1);
	glm::vec3 color = glm::vec3(1, 1, 1);
	float powerWatts = 22; // default is car spot light 
	float size; // angle 1 to 180
	float blend; // 0 to 1
};

class RAreaLight {
	// lmao not right now
};

class RDirectionalLight {
public:
	glm::vec3 direction = glm::vec3(1, 1, 1);
	glm::vec3 color;
	float powerWattsPerMtr2;
	float anglularDiameter;
};
