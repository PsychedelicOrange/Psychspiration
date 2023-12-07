#include<RInput.h>
#include<GLFW/glfw3.h>
void Input::updatedeltaTime()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
void Input::tick()
{
	for (auto item : repeatCallbacks)
	{
		auto control =	item.first;
		if (keyStates[control] == 2 || keyStates[control] == 1)
			for (auto callback : item.second)
				callback();
	}
	for (auto tickCallback : perTickCallbacks)
		tickCallback();
}

//for both mouse and keyboard

 void Input::registerPerTickCallback(std::function<void()> callback)
{
	perTickCallbacks.push_back(callback);
}
 void Input::registerKeyCallback(std::string control, int action, std::function<void()> callback)
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
 void Input::registerMouseMovementCallback(std::function<void(double xpos, double ypos)> callback)
{
	mouseMovementCallbacks.push_back(callback);
}
 void Input::registerMouseScrollCallback(std::function<void(double xoffset, double yoffset)> callback)
{
	mouseScrollCallbacks.push_back(callback);
}

// private do not call (these are GLFW callbacks)

 void Input::key_callback(int key, int scancode, int action, int mods)
{
	 // DEBUG ONLY
	 if (!recieveKeyboardCallbacks)return;
	try {
		std::string control = settings.GLFWKeysToControl.at(key);
		keyStates[control] = action;
		eventHandler.fireCallback(control, action);
	}
	catch (std::out_of_range)
	{
		std::cout << "No key Assigned !";
	}
}

// private do not call (these are GLFW callbacks)

 void Input::mouse_button_callback(int button, int action, int mods)
{
	 // DEBUG ONLY
	//if (!recieveMouseCallbacks)return;	
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

 void Input::mouse_movement_callback(double xpos, double ypos)
{
	 // DEBUG ONLY
	if (!recieveMouseCallbacks)return;
	for (auto action : mouseMovementCallbacks)
	{
		action(xpos, ypos);
	}
}

// private do not call (these are GLFW callbacks)

 void Input::scroll_callback(double xoffset, double yoffset)
{
	for (auto action : mouseScrollCallbacks)
	{
		action(xoffset, yoffset);
	}
}

