/*
See the link below for variable meanings
https://docs.blender.org/manual/en/latest/render/lights/light_object.html
*/
#pragma once
#include <enums.h>
#include <glm/vec3.hpp>
class Light
{
	LightType lightType;
	glm::vec3 position;
	glm::vec3 direction;
	float radius;
	float power;
	float Intensity;
	glm::vec3 color;
	float angleOuterCone;
	float blend;
};