#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
class EventHandler
{
//public:
//    typedef void (*action_callback_function)();
//    std::unordered_map<std::string, action_callback_function> controlTable;
//    void* curr_instance = nullptr;
//    template <typename T> void registerCallback(std::string action, T action_callback,void *instance)
//    {
//        std::cout << "makes me wet";
//        curr_instance = instance;
//        (instance)->action_callback(instance);
//        //controlTable.insert({ action,action_callback });
//    }
//    void fireCallback(std::string action)
//    {
//        (controlTable[action])();
//    }
public:
	EventHandler() {
	}
	std::unordered_map <std::string, std::function<void()>> controlTable;
	void registerCallback(std::string action, std::function<void()> callback)
	{
		controlTable.insert({ action,callback });
	}
	void fireCallback(std::string action)
	{
		std::cout << "\nButton Press Detected: " << action;
		if (controlTable[action])
			(controlTable[action])();
		else
			std::cout << "\nError: " << action << " action not registered anywhere !";
	}
	//void processHoldKeys(GLFWwindow* window,//inherit a common class with process keyboard functions)
	//{
	//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//		camera.ProcessKeyboard(FORWARD, deltaTime);
	//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//		camera.ProcessKeyboard(BACKWARD, deltaTime);
	//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//		camera.ProcessKeyboard(LEFT, deltaTime);
	//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//		camera.ProcessKeyboard(RIGHT,deltaTime);
	//}
};
