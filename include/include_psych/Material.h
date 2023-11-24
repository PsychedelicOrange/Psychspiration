#pragma once
#include <RTexture.h>
#include <vector>
#include <glm/glm.hpp>
using std::vector;
class Material
{
public:
	vector<RTexture*> TexAlbedo, TexNormal, TexRoughMetal, TexEmmisive, TexOther;
	glm::vec3 Albedo{0.8,0.8,0.8}, Emmisive{ 0,0,0 };
	float Metallic{0}, Roughness{0.5};
	AlphaType aplhaType{AlphaType::OPAQUE};
};