#pragma once
#include <ResourceManager.h>
#include <Texture.h>
#include <string>
class TextureObject
{
public:
	Texture* tex;
	ResourceManager* resourceManager;
	TextureObject(std::string path){
		tex = resourceManager->getTexture(path);
	}
};
