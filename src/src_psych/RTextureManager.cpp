#include <RTextureManager.h>
RTextureManager* RTextureManager::instance;
RTextureManager* RTextureManager::getInstance()
{
	if (instance == NULL) {
		return instance = new RTextureManager();
	}
	else
		return instance;
}

RTexture* RTextureManager::getTexture(string path, TextureType type)
{
	if (Textures.empty())
	{
		Textures.emplace(path, builder.build(path, type));
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
