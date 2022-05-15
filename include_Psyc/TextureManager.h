#pragma once
#include <Texture.h>
#include <unordered_map>
#include <string>
class TextureManager
{
public:
	std::unordered_map<std::string, Texture*> Textures;
	Texture* getTexture(std::string path);
	
};
