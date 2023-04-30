#pragma once
#include <string>
#include <unordered_map>
#include <Controls.h>
#include <GLFW/glfw3.h>
using std::string;
using std::unordered_map;
class Settings {
public:
//default settings
	//video
	int resolution[2] = {1920,1080};
	string display_mode = "windowed";
	int anti_aliasingMSAA = 9;
	
	//controls
	unordered_map<Controls, int> controlToGLFWKeys = {
		{Controls::FORWARD_KEY,GLFW_KEY_W},
		{Controls::BACKWARD_KEY,GLFW_KEY_S},
		{Controls::RIGHT_KEY,GLFW_KEY_D},
		{Controls::LEFT_KEY,GLFW_KEY_A}
	};
};