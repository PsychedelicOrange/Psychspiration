#pragma once
#include <Shader.h>
#include <RMesh.h>
class BatchStatic {
	Shader* shader;
	vector<RMesh*> meshes;
};