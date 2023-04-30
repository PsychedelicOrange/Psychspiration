#pragma once
#include <assimp/pbrmaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Material.h>
#include <RTextureManager.h>
#include <unordered_map>
using std::unordered_map;
//#include<assimp/material.h>
class RMaterialBuilder
{
	Material material;
	aiMaterial* aimaterial;
	RTextureManager* textureManager;
public:
	RMaterialBuilder() {
		textureManager = RTextureManager::getInstance();
	}
	const unordered_map<TextureType, aiTextureType> Ttype 
	{{TextureType::Albedo,aiTextureType_DIFFUSE},
	{TextureType::MetallicRoughness,aiTextureType_UNKNOWN},
	{TextureType::Normal,aiTextureType_NORMALS},
	{TextureType::Emmisive,aiTextureType_EMISSIVE} };

	Material build(aiMaterial* aimaterial)
	{
		this->aimaterial = aimaterial;
		loadMaterialValues();
		loadMaterialTextures();
		return material;
	}
private:
	void loadMaterialValues()
	{
		aiMaterial* aimaterial_temp = aimaterial;
		aiColor3D color(0.f, 0.f, 0.f);
		aiString aiAlphaMode;
		ai_real metallic, roughness, emmisive;
		aimaterial_temp->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		aimaterial_temp->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
		aimaterial_temp->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
		aimaterial_temp->Get(AI_MATKEY_COLOR_EMISSIVE, emmisive);
		aimaterial_temp->Get(AI_MATKEY_GLTF_ALPHAMODE, aiAlphaMode);
		material.Metallic = (float)metallic;
		material.Roughness = (float)roughness;
		material.Emmisive = (float)emmisive;
		material.Albedo = glm::vec3(color.r, color.g, color.b);
		if (aiAlphaMode.C_Str() == "OPAQUE")
			material.aplhaType = AlphaType::OPAQUE;
		else if (aiAlphaMode.C_Str() == "BLEND")
			material.aplhaType = AlphaType::BLEND;
		else
			material.aplhaType = AlphaType::MASK;
	}
	void loadMaterialTextures()
	{
		RTextureManager* textureManager = RTextureManager::getInstance();
		aiMaterial* aimaterial_temp = aimaterial;
		for (auto type : Ttype)
		{
			vector<RTexture*> textures;
			aiString str;
			for (unsigned int i = 0; i < aimaterial_temp->GetTextureCount(type.second); i++)
			{
				
				aimaterial_temp->GetTexture(type.second, i, &str);

				RTexture* t = textureManager->getTexture(str.C_Str());
				t->textureType = type.first;
				textures.push_back(t);
			}
			switch (type.first)
			{
			case TextureType::Albedo:
				material.TexAlbedo = textures; break;
			case TextureType::MetallicRoughness:
				material.TexMetallicRoughness = textures; break;
			case TextureType::Normal:
				material.TexNormal = textures; break;
			case TextureType::Emmisive:
				material.TexEmmisive = textures; break;
			}
		}
		
	}
};