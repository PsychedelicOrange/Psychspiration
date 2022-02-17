#pragma once
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