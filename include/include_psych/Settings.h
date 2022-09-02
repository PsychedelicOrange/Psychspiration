#pragma once
#include <EventHandler.h>
#include <vector>
#include <string>

class Settings {
public:
	unsigned int SCR_WIDTH;
	unsigned int SCR_HEIGHT;
	unsigned int monitor; // 0 for windowed, 1 for fullscreen
	std::wstring wstr;
	std::string relativePath;
	static std::string realtivePathStatic;
	std::string resourcePath; // scene name
	bool hdr;
	float exposure;
	unsigned int MSAA;
	unsigned int AF;
	void update();
	Settings(EventHandler* eventHandler)
	{
		SCR_WIDTH = 1920;
		SCR_HEIGHT = 1080;
		exposure = 0.5;
		resourcePath = "instanced_flowers";
		hdr = 1;
		//update();
		eventHandler->registerCallback("Update_Settings", [=]() {update(); });
	}
};