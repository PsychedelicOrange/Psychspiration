#include <RModelManager.h>
RModelManager* RModelManager::instance;
RModelManager* RModelManager::getInstance()
{
	if (instance == NULL)
		return instance = new RModelManager();
	else
		return instance;
}

RModel* RModelManager::getModel(string path)
{
	auto search = Models.find(path);
	if (search != Models.end())
	{
		return search->second;
	}
	else
	{
		Models.emplace(path, rModelBuilder->build("\\Models\\" + path + ".gltf"));
		return Models[path];
	}
}
