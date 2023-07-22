#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <enums.h>
using std::string;
using std::unordered_map;
class Settings {
public:
	Settings() {
    
	}
	//video
	int resolution[2] = {1920,1080};
	DisplayType display_mode = DisplayType::WINDOWED;
	int anti_aliasingMSAA = 8;
	//controls
	unordered_map<int,string> GLFWKeysToControl = {
		{GLFW_KEY_W,"FORWARD_KEY"},
		{GLFW_KEY_S,"BACKWARD_KEY"},
		{GLFW_KEY_D,"RIGHT_KEY"},
		{GLFW_KEY_A,"LEFT_KEY"},
		{GLFW_KEY_ESCAPE,"EXIT_KEY"},
		{GLFW_KEY_SPACE,"PLAY_KEY"}
	};
};