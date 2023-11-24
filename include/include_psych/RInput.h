#pragma once
//#include <Events.h>
#include <EventHandler.h>
#include <RSettings.h>
class Input {
public:
	
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	
	bool recieveMouseCallbacks = true;
	bool recieveKeyboardCallbacks = true;
	std::vector<std::function<void()>> perTickCallbacks;

	std::unordered_map<string, int> keyStates; //store for repeat
	std::unordered_map<string,std::vector<std::function<void()>>> repeatCallbacks;

	std::vector<std::function<void(double xpos, double ypos)>> mouseMovementCallbacks;
	std::vector<std::function<void(double xoffset, double yoffset)>> mouseScrollCallbacks;
	
	EventHandler eventHandler; // stores button key,action pairs with their corresponding callbacks.
	Settings& settings;
	Input(Settings& settings): settings(settings){}

	void tick();
	void updatedeltaTime();
	/*Register various callbacks*/

	//for both mouse and keyboard
	// if action 0 means fire on released, 1 means fire on pressed, and 2 means fire on repeat, and 3 means fire on held(continuosly).
	void registerPerTickCallback(std::function<void()> callback);
	// if action 0 means fire on released, 1 means fire on pressed, and 2 means fire on repeat, and 3 means fire on held(continuosly).
	void registerKeyCallback(std::string control, int action, std::function<void()> callback);
	// if action 0 means fire on released, 1 means fire on pressed, and 2 means fire on repeat, and 3 means fire on held(continuosly).
	void registerMouseMovementCallback(std::function<void(double xpos, double ypos)> callback);
	// if action 0 means fire on released, 1 means fire on pressed, and 2 means fire on repeat, and 3 means fire on held(continuosly).
	void registerMouseScrollCallback(std::function<void(double xoffset, double yoffset)> callback);
	// private do not call (these are GLFW callbacks)
	void key_callback(int key, int scancode, int action, int mods);
	// private do not call (these are GLFW callbacks)
	void mouse_button_callback(int button, int action, int mods);
	// private do not call (these are GLFW callbacks)
	void mouse_movement_callback(double xpos, double ypos);
	// private do not call (these are GLFW callbacks)
	void scroll_callback(double xoffset, double yoffset);

};