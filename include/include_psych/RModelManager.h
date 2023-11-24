#pragma once
#include <map>
#include <RModel.h>
#include <RModelBuilder.h>
using std::map;
class RModelManager
{
	map<string, RModel*> Models;
	RModelBuilder* rModelBuilder;
	static RModelManager* instance;
	RModelManager() { rModelBuilder = new RModelBuilder(); }
public:
	// deleting copy constructor
	RModelManager(RModelManager& obj)
		= delete;

	static RModelManager* getInstance();
	RModel* getModel(string path);
};
