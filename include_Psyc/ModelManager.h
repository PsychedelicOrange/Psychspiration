#pragma once
#include <unordered_map>
#include <map>
#include <Model.h>
#include <string>
class ModelManager
{
public:
	std::map<std::string, Model*> Models;
	Model* getModel(std::string path);
};