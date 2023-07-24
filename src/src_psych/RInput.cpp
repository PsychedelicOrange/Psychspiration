#include<RInput.h>
#include<GLFW/glfw3.h>

void Input::tick()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

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

