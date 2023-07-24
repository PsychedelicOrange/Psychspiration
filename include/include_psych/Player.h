#pragma once
#include <iostream>
#include <RObject.h>
#include <RInput.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include<RCameraObject.h>
class Player
{
public:
	RObject* obj; //models and physics
	btRigidBody* body;
	Input& input;
	RCameraObject* camera;
	// transform
	glm::vec3 forward;

	Player(RObject* obj,Input& input,RCameraObject* camera): obj(obj),input(input),body(obj->rigidBody),camera(camera),forward(glm::vec3(0,0,-1))
	{
		setUpEvents(input);
	}
	void updateCamera(float minDistance)
	{
		glm::vec3 position(obj->transform[3]);
		btVector3 velocity = { obj->rigidBody->getLinearVelocity().x(),obj->rigidBody->getLinearVelocity().y(),obj->rigidBody->getLinearVelocity().z() };
		float distance = std::max(std::sqrt(velocity.length()), minDistance);
		/*velocity = glm::normalize(velocity);
		if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0)
			velocity = initialFront;
		camera->Front =velocity;*/
		camera->Position = position - (forward * distance);
		camera->Front = position - camera->Position;
		camera->Position = position - (forward * distance);
	}
	void applyForce(int dir,float deltatime)
	{
		int magnitude = 50;
		body->activate(true);
		btVector3 front = { forward.x , forward.y, forward.z};
		btVector3 Up = { camera->Up.x,camera->Up.y,camera->Up.z };
		btVector3 force;
		if (dir == 0)
			force = front;
		else if (dir == 1)
			force = -front;
		else if (dir == 2)
			force = -btCross(front, Up);
		else if (dir == 3)
			force = btCross(front, Up);
		force = force.normalized() * magnitude;
		body->applyCentralForce(force);
	}
	void setUpEvents(Input& input)
	{
		input.registerKeyCallback("FORWARD_KEY", 3, [this, &input]() {
			//std::cout << "hi" << input.deltaTime;
			applyForce(0, input.deltaTime);
			});
		input.registerKeyCallback("BACKWARD_KEY", 3, [this, &input]() {
			applyForce(1, input.deltaTime);
			});
		input.registerKeyCallback("LEFT_KEY", 3, [this, &input]() {
			applyForce(2, input.deltaTime);
			});
		input.registerKeyCallback("RIGHT_KEY", 3, [this, &input]() {
			applyForce(3, input.deltaTime);
			});
		input.registerPerTickCallback([this]() {tickUpdate(); });
	}
	void tickUpdate()
	{
		updateCamera(5);
		body->applyCentralForce(btVector3(0,9,0));
	}
};
