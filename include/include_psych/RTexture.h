#pragma once
#include <string>
#include <enums.h>
using std::string;
struct RTexture {
	int ID; // openGL Texture name 
	TextureType textureType;
	std::string path;
};