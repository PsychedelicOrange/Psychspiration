#pragma once
#include <iostream>
#include <Object.h>
#include <Window.h>
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
	void ProcessKeyboard(GLFWwindow* window)
	{
		glm::vec3 translate{0.f}; // do the inheritance wala thing we dont want GLFWwindow here we want it in eventHandler	
		float velocity = 1.0f * eventHandler->deltaTime;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			translate.x +=  velocity;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			translate.x -= velocity;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			translate.y -=  velocity;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			translate.y += velocity;
		obj->transform = glm::translate(obj->getTransform(), translate);
	}
	void setUpEvents()
	{
		eventHandler->registerCallback("Player_Move_Forward", [=]() {
			std::cout << "\n\nplayer moved\n\n";
			
			obj->transform = glm::translate(obj->getTransform(), glm::vec3(1, 0, 0));
			});
	}
	
};
