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
		Models.emplace(path, new Model("\\Models\\" + path + ".gltf"));
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
		hulls.emplace(path, new Model("\\Hulls\\" + path + ".glb"));
		return hulls[path];
	}
}

ModelManager::~ModelManager()
{
	for (auto elm : Models)
		delete elm.second;
	for (auto elm : hulls)
		delete elm.second;
}
