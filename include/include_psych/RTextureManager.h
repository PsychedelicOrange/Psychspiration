#pragma once
#include<RTexture.h>
#include<unordered_map>
#include<RTextureBuilder.h>
class RTextureManager
{
	std::unordered_map<string, RTexture*> Textures;
	RTextureBuilder builder;

	// Singleton class
	static RTextureManager* instance;
	RTextureManager() {};
public:
	std::unordered_map<string, RTexture*> getAll()
	{
		return Textures;
	}
	RTextureManager(RTextureManager& obj)
		= delete;
	static RTextureManager* getInstance();
	RTexture* getTexture(string path, TextureType type = TextureType::Other);
};
