#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <utility>
using std::string;
using std::pair;
using std::unordered_map;
using std::vector;
using std::function;
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
	unordered_map <string,vector<function<void()>>> controlTable;
	// action GLFW_RELEASE = 0, GLFW_PRESS = 1
	void registerCallback(std::string key,int action, std::function<void()> callback)
	{
		key += std::to_string(action);
		if (controlTable.find(key) != controlTable.end())
			controlTable[key].push_back(callback);
		else
			controlTable[key] = vector<function<void()>>{ callback };
	}
	void fireCallback(std::string key,int action)
	{
		key += std::to_string(action);
		//std::cout << "\nButton Press Detected: " << key;
		if (controlTable.find(key) != controlTable.end())
			for (auto callback : controlTable[key])
				callback();
		else
			std::cout << "\nError: " << key << " Key not registered anywhere !";
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
