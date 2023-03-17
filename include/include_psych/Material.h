#pragma once
#include <RTexture.h>
#include <vector>
#include <glm/glm.hpp>
using std::vector;
class Material
{
public:
	vector<RTexture*> TexAlbedo, TexNormal, TexMetallicRoughness, TexEmmisive, TexOther;
	glm::vec3 Albedo; float Metallic, Roughness, Emmisive; AlphaType aplhaType;
};