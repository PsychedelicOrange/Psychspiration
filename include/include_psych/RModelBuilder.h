#pragma once
#include <ASSIMPimporter.h>
#include <RModel.h>
class RModelBuilder
{
public:
	ASSIMPimporter* assimp;
	RModelBuilder() { assimp = new ASSIMPimporter(); }
	RModel* build(string path)
	{
		return assimp->load(path);
	}
};
