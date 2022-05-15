#pragma once
#include <unordered_map>
#include <Model.h>
#include <string>
class ModelManager
{
public:
	std::unordered_map<std::string, Model*> Models;
	Model* getModel(std::string path);
};