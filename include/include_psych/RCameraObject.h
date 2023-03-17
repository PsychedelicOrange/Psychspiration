#pragma once
#include "../RCamera.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
class RCameraObject
{
	RCamera* camera;
public:
	glm::vec3 Position, Front, Up, Right;
	glm::mat4 view;
	glm::mat4 projection;
	RCameraObject(RCamera* camera,glm::vec3 Position = glm::vec3(10,10,10),glm::vec3 Up = glm::vec3(0,1,0)): Position(Position),Up(Up),camera(camera)
	{
		innitMatrices();
	}
	const RCamera getCamera() 
	{
		return *camera;
	}
	void setCamera(RCamera* camera)
	{
		this->camera = camera;
		innitMatrices();
	}
	void innitMatrices()
	{
		projection = glm::perspective(glm::radians(camera->yfov), camera->aspectRatio, camera->znear, camera->zfar);
		view = glm::lookAt(Position, Position + Front, Up);
	}
	
};