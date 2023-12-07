#pragma once
#include <RInput.h>
class State
{
public:
	//toggle
//	bool shadows = false; 
//	bool normals = true; 
//	bool cameraDebugBool = false; 
	//hold
	bool play = false;

	//windows 
	bool show_menu = false; 
	//bool show_renderer = false; 
	
	// last
	bool firstMouse = true;
	float lastFrame = 0.0f;
	
	// exposure and hdr
	int hdr = 1;
	float exposure =0.5;
	void update()
	{

	}
	State(Input& input){
		input.registerKeyCallback("MIDDLE_CLICK_KEY", 0, [this, &input]() { 
			show_menu = !show_menu; // toggle menu
			if(show_menu)
			{
				//input.recieveKeyboardCallbacks = false;
				input.recieveMouseCallbacks = false;
			}
			else
			{
				//input.recieveKeyboardCallbacks = true;
				input.recieveMouseCallbacks = true;
			}
			});
		input.registerKeyCallback("PLAY_KEY", 0, [this]() {play = !play; });
	}
};
