#pragma once
#include <RModel.h>
#include <glm/common.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <string>
#include <Hull.h>
using std::string;
class RObject {
public:
	string name,path;
	RModel* model;
	glm::mat4 transform;

	bool dynamic = 0;
	std::vector<Hull*> hulls;
	btRigidBody* rigidBody;
	glm::vec3 localScale = glm::vec3(1,1,1);
};