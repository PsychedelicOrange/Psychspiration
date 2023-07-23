#pragma once
#include "RCamera.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <RInput.h>
//#include <Events.h>
class RCameraObject
{
	RCamera* camera;
public:
	
	glm::vec3 Position, Front, Up, Right,WorldUp;
	glm::mat4 view;
	glm::mat4 projection;
	float MouseSensitivity = 0.2f;
	float MovementSpeed = 2.5f;
	// euler Angles
	float Yaw = -90.0f;
	float Pitch = 0;

	RCameraObject(RCamera* camera , Input& input, glm::vec3 Position = glm::vec3(0,0,1),glm::vec3 Up = glm::vec3(0,1,0),glm::vec3 Front = glm::vec3(0,0,-1)): Position(Position),Up(Up),Front(Front),camera(camera)
	{
		input.registerMouseMovementCallback([this](double xpos, double ypos) {
			processMouseInput(xpos, ypos);
			});
		input.registerKeyCallback("FORWARD_KEY", 3, [this, &input]() {
			//std::cout << "hi" << input.deltaTime;
			ProcessKeyboard(0, input.deltaTime);
			});
		input.registerKeyCallback("BACKWARD_KEY", 3, [this, &input]() {
			ProcessKeyboard(1, input.deltaTime);
			});
		input.registerKeyCallback("LEFT_KEY",3, [this, &input]() {
			ProcessKeyboard(2, input.deltaTime);
			});
		input.registerKeyCallback("RIGHT_KEY",3, [this, &input]() {
			ProcessKeyboard(3, input.deltaTime);
			});
		WorldUp = Up;
		Right = glm::cross(Front, WorldUp);
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
	}
	glm::mat4 getView()
	{
		return view = glm::lookAt(Position, Position + Front, Up);
	}
	void processMouseInput(double xoffset, double yoffset)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;


		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
	void ProcessKeyboard(int direction, float deltaTime)
	{
		//std::cout << "Fired !";
		float velocity = MovementSpeed * deltaTime;
		if (direction == 0)
			Position += Front * velocity;
		if (direction == 1)
			Position -= Front * velocity;
		if (direction == 2)
			Position -= Right * velocity;
		if (direction == 3)
			Position += Right * velocity;
	}
};