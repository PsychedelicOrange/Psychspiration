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
	//video
	int resolution[2] = {1920,1080};
	DisplayType display_mode = DisplayType::WINDOWED;
	int anti_aliasingMSAA = 8;
	//controls defualt
	unordered_map<string, int> ControlToGLFWKeys = {
		{"FORWARD_KEY", GLFW_KEY_W},
		{"BACKWARD_KEY", GLFW_KEY_S},
		{"RIGHT_KEY", GLFW_KEY_D},
		{"LEFT_KEY", GLFW_KEY_A},
		{"EXIT_KEY", GLFW_KEY_ESCAPE},
		{"PLAY_KEY", GLFW_KEY_SPACE}
	};
	unordered_map<int,string> GLFWKeysToControl = {
		{GLFW_KEY_W,"FORWARD_KEY"},
		{GLFW_KEY_S,"BACKWARD_KEY"},
		{GLFW_KEY_D,"RIGHT_KEY"},
		{GLFW_KEY_A,"LEFT_KEY"},
		{GLFW_KEY_ESCAPE,"EXIT_KEY"},
		{GLFW_KEY_SPACE,"PLAY_KEY"}
	};
	unordered_map<string, int> ControlToGLFWMouseKeys = {
		{"FORWARD_KEY", GLFW_KEY_W},
		{"BACKWARD_KEY", GLFW_KEY_S},
		{"RIGHT_KEY", GLFW_KEY_D},
		{"LEFT_KEY", GLFW_KEY_A},
		{"EXIT_KEY", GLFW_KEY_ESCAPE},
		{"PLAY_KEY", GLFW_KEY_SPACE}
	};
	unordered_map<int, string> GLFWMouseKeysToControl = {
		{GLFW_KEY_W,"FORWARD_KEY"},
		{GLFW_KEY_S,"BACKWARD_KEY"},
		{GLFW_KEY_D,"RIGHT_KEY"},
		{GLFW_KEY_A,"LEFT_KEY"},
		{GLFW_KEY_ESCAPE,"EXIT_KEY"},
		{GLFW_KEY_SPACE,"PLAY_KEY"}
	};
};