#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
class Settings {
public:
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
	unsigned int monitor; // 0 for windowed, 1 for fullscreen
	std::string resourcePath; // path to scene for now
	std::string shaderPath; //path to shader
	unsigned int hdr;
	float exposure;
	unsigned int MSAA;
	unsigned int AF;
	void update();
	Settings()
	{
		update();
	}
};