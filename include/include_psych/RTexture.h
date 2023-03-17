#pragma once
#include <string>
#include <enums.h>
using std::string;
class RTexture {
public:
	int ID; // openGL Texture name
	string path;
	TextureType textureType;
};