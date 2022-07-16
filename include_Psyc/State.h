#pragma once
#include <EventHandler.h>
class State
{
public:
	//toggle
	bool shadows = true; 
	bool normals = true; 
	bool cameraDebugBool = false; 
	
	//hold
	bool play = false;

	//windows 
	bool show_menu = true;
	bool show_renderer = false;
	
	// last
	bool firstMouse = true;
	float lastFrame = 0.0f;
	
	void update()
	{

	}
	State(EventHandler* eventHandler){
		eventHandler->registerCallback("Toggle_Camera", [=]() {
			cameraDebugBool = !cameraDebugBool;
			});
		eventHandler->registerCallback("Toggle_Debug", [=]() {
			show_menu = !show_menu;
			});
	}
};
