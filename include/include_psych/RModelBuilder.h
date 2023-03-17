#pragma once
#include <ASSIMPimporter.h>
#include <RTextureBuilder.h>
#include <RModel.h>
class RModelBuilder
{
public:
	ASSIMPimporter assimp;
	RModel* build(string path)
	{
		return assimp.load(path);
	}
};
