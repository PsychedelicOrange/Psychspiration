#pragma once
#include<RTexture.h>
#include<unordered_map>
#include <RTextureBuilder.h>
class RTextureManager
{
	std::unordered_map<string, RTexture*> Textures;
	RTextureBuilder builder;

	// Singleton class
	static RTextureManager* instance;
	RTextureManager() {};
public:
	RTextureManager(const RTextureManager& obj)
		= delete;
	static RTextureManager* getInstance()
	{
		if (instance == NULL)
			instance = new RTextureManager();
		else
			return instance;
	}
	RTexture* getTexture(string path,TextureType type = TextureType::Other)
	{
		if (Textures.empty())
		{
			Textures.emplace(path, builder.build(path,type));
			return Textures[path];
		}
		else
		{
			auto search = Textures.find(path);
			if (search != Textures.end())
			{
				return search->second;
			}
			else
			{
				Textures.emplace(path, builder.build(path, type));
				return Textures[path];
			}
		}

	}
};
