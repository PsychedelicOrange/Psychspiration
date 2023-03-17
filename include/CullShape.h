#pragma once
#include <glm/vec3.hpp>
#include "RCamera.h"
class CullShape
{

};
class Sphere : CullShape
{
	float radius;
	glm::vec3 position;
	Sphere(float radius, glm::vec3 position) : radius(radius), position(position) {}
};
class Frustum : CullShape
{
	float halftan, nhh, nhw, fhh, fhw, near_plane, far_plane, diagnolFrustum;
	Frustum()
	{
		
	}
};