#include <TextureManager.h>
Texture* TextureManager::getTexture(std::string path)
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