#pragma once
#include "RCamera.h"
#include <glm/glm.hpp>
#include <RInput.h>
//#include <Events.h>
class RCameraObject
{
public:
	RCamera* camera;
	glm::vec3 Position, Front, Up, Right,WorldUp;
	glm::mat4 view;
	glm::mat4 projection;
	float MouseSensitivity = 0.2f;
	float MovementSpeed = 2.5f;
	// euler Angles
	float Yaw = -90.0f;
	float Pitch = 0;
	// mouse
	float lastX;
	float lastY;
	bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	
	RCameraObject(RCamera* camera, Input& input, glm::vec3 Position = glm::vec3(0, 0, 1), glm::vec3 Up = glm::vec3(0, 1, 0), glm::vec3 Front = glm::vec3(0, 0, -1));

	const RCamera getCamera();
	void setCamera(RCamera* camera);
	void innitMatrices();
	glm::mat4 getView();
	void updateCamera(double xpos, double ypos);
	void processMouseInput(double xoffset, double yoffset);
	void ProcessKeyboard(int direction, float deltaTime);
};