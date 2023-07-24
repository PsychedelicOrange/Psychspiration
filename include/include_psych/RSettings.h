#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <enums.h>
using std::string;
using std::unordered_map;
class Settings {
public:
	//video
	int resolution[2] = {1920,1080};
	DisplayType display_mode = DisplayType::WINDOWED;
	int anti_aliasingMSAA = 8;

	Settings();
	unordered_map<string, int> ControlToGLFWKeys;
	unordered_map<int,string> GLFWKeysToControl;
	unordered_map<string, int> ControlToGLFWMouseKeys;
	unordered_map<int, string> GLFWMouseKeysToControl;
};