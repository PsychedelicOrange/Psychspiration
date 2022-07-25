#include <ModelManager.h>
Model* ModelManager::getModel(std::string path)
{
	auto search = Models.find(path);
	if (search != Models.end())
	{
		return search->second;
	}
	else
	{
		Models.emplace(path, new Model("Resources/Models/" + path + ".gltf"));
		return Models[path];
	}
}

Model* ModelManager::getHull(std::string path)
{
	auto search = hulls.find(path);
	if (search != hulls.end())
	{
		return search->second;
	}
	else
	{
		hulls.emplace(path, new Model("Resources/Hulls/" + path + ".glb"));
		return hulls[path];
	}
}
