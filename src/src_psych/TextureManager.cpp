#include <TextureManager.h>
Texture* TextureManager::getTexture(std::string path)
{
	if(Textures.empty())
	{
		Textures.emplace(path, new Texture(path));
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
			Textures.emplace(path, new Texture(path));
			return Textures[path];
		}
	}
	
}

Texture* TextureManager::getTextureEmbedded(const aiTexture* path, std::string typeName)
{
	return new Texture(path, typeName);
}
