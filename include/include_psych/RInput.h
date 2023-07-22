#pragma once
#include <EventHandler.h>
#include <RSettings.h>
class Input {
public:
	Settings settings;
	Input(Settings settings): settings(settings) {
	}
	EventHandler eventHandler;
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

		try {
			std::string hi = settings.GLFWKeysToControl.at(key);
			eventHandler.fireCallback(hi);
		}
		catch (std::out_of_range)
		{
			std::cout << "No key Assigned !";
		}
	}

};