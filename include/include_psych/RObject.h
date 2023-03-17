#pragma once
#include <RModel.h>
#include<glm/common.hpp>
class RObject {
	string name,path;
	RModel* model;
	glm::mat4 transform;
};