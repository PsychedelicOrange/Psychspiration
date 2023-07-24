#pragma once
//#include <Events.h>
#include <EventHandler.h>
#include <RSettings.h>
class Input {
public:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	
	std::vector<std::function<void()>> perTickCallbacks;

	std::unordered_map<string, int> keyStates; //store for repeat
	std::unordered_map<string,std::vector<std::function<void()>>> repeatCallbacks;

	std::vector<std::function<void(double xpos, double ypos)>> mouseMovementCallbacks;
	std::vector<std::function<void(double xoffset, double yoffset)>> mouseScrollCallbacks;
	
	EventHandler eventHandler; // stores button key,action pairs with their corresponding callbacks.
	Settings& settings;
	Input(Settings& settings): settings(settings){}

	void tick();

	/*Register various callbacks*/

	//for both mouse and keyboard
	// if action 0 means fire on released, 1 means fire on pressed, and 2 means fire on repeat, and 3 means fire on held(continuosly).
	void registerPerTickCallback(std::function<void()> callback)
	{
		perTickCallbacks.push_back(callback);
	}
	void registerKeyCallback(std::string control,int action, std::function<void()> callback)
	{
		if (action == 3)
		{
			if (repeatCallbacks.find(control) != repeatCallbacks.end())
				repeatCallbacks[control].push_back(callback);
			else
				repeatCallbacks[control] = { callback };
		}
		else
			eventHandler.registerCallback(control, action, callback);
	}
	void registerMouseMovementCallback(std::function<void(double xpos, double ypos)> callback)
	{
		mouseMovementCallbacks.push_back(callback);
	}
	void registerMouseScrollCallback(std::function<void(double xoffset, double yoffset)> callback)
	{	
		mouseScrollCallbacks.push_back(callback);
	}
	// private do not call (these are GLFW callbacks)
	void key_callback(int key, int scancode, int action, int mods)
	{
		
		try {
			std::string control = settings.GLFWKeysToControl.at(key);
			keyStates[control] = action;
			eventHandler.fireCallback(control,action);
		}
		catch (std::out_of_range)
		{
			std::cout << "No key Assigned !";
		}
	}
	// private do not call (these are GLFW callbacks)
	void mouse_button_callback( int button, int action, int mods)
	{
		try {
			std::string control = settings.GLFWMouseKeysToControl.at(button);
			keyStates[control] = action;
			eventHandler.fireCallback(control, action);
		}
		catch (std::out_of_range)
		{
			std::cout << "No key Assigned !";
		}
	}
	// private do not call (these are GLFW callbacks)
	void mouse_movement_callback(double xpos, double ypos)
	{
		for (auto action : mouseMovementCallbacks)
		{
			action(xpos,ypos);
		}
	}
	// private do not call (these are GLFW callbacks)
	void scroll_callback( double xoffset, double yoffset)
	{
		for (auto action : mouseScrollCallbacks)
		{
			action(xoffset, yoffset);
		}
	}

};