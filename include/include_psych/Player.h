#pragma once
#include <iostream>
#include <RObject.h>
#include <RInput.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <RCameraObject.h>
class Player
{
public:
	RObject* obj; //models and physics
	btRigidBody* body;
	Input& input;
	RCameraObject* camera;
	float angle_of_attack;
	// transform
	glm::vec3 forward;
	btVector3 velocity;
	glm::vec3 up;
	

	Player(RObject* obj, Input& input, RCameraObject* camera);
	void updateCamera(glm::vec3 up,float minDistance);
	void applyForce(int dir, float deltatime);
	void applyTurnTorque(int dir, float deltatime);
	void setUpEvents(Input& input);
	void tickUpdate();
};
