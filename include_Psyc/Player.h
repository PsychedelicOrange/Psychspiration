#pragma once
#include <iostream>
#include <Object.h>
#include <EventHandler.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

class Player
{
public:
	Object* obj; //models and physics
	EventHandler* eventHandler; // controls
	Player(EventHandler* eventHandler) { this->eventHandler = eventHandler; }
	Player(Object* obj,EventHandler* eventHandler)
	{
		this->obj = obj;
		this->eventHandler = eventHandler;
	}
	void ProcessKeyboard()
	{
		glm::vec3 translate{0.f}; // do the inheritance wala thing wwe dont want GLFWwindow here we want it in eventHandler	
		float velocity = 1.0f * eventHandler->deltaTime;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			translate.x +=  velocity;
		if (direction == BACKWARD)
			translate.x -= velocity;
		if (direction == LEFT)
			translate.y -=  velocity;
		if (direction == RIGHT)
			translate.y += velocity;
	}
	void setUpEvents()
	{
		eventHandler->registerCallback("Player_Move_Forward", [=]() {
			std::cout << "\n\nplayer moved\n\n";
			
			obj->transform = glm::translate(obj->getTransform(), glm::vec3(1, 0, 0));
			});
	}
	
};
