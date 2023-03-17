#pragma once
#include <map>
#include <RModel.h>
#include <RModelBuilder.h>
using std::map;
class RModelManager
{
	map<string, RModel*> Models;
	RModelBuilder rModelBuilder;
	static RModelManager* instance;
	RModelManager() {}
public:
	// deleting copy constructor
	RModelManager(const RModelManager& obj)
		= delete;

	static RModelManager* getInstance()
	{
		if (instance == NULL)
			return instance = new RModelManager();
		else
			return instance;
	}
	RModel* getModel(string path)
	{
		auto search = Models.find(path);
		if (search != Models.end())
		{
			return search->second;
		}
		else
		{
			Models.emplace(path, rModelBuilder.build("\\Models\\" + path + ".gltf"));
			return Models[path];
		}
	}
};
