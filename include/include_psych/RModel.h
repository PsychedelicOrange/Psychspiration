#pragma once
#include<RMesh.h>
class RModel
{
public:
	vector<RMesh*> meshes;
	string path;
	friend std::ostream& operator<<(std::ostream& os, const RModel* model);
};
