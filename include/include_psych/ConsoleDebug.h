#pragma once
#include <iostream>
#include <RObject.h>
#include "glm/ext.hpp"  // for to_string
class ConsoleDebug {
public :
	void log(RObject* object)
	{
		std::cout << "Object Information: \n";
		std::cout << "\tObject Name: " << object->name << std::endl;
		std::cout << "\tObject Model Path : " << object->path << std::endl;
		std::cout << "\tDynamic: " << object->dynamic << std::endl;
		std::cout << "\tNo. of Hulls : " << object->hulls.size() << std::endl;
	}
	void log(RModel* model)
	{
		std::cout << "Model Information: \n";
		std::cout << "\t Model Name: " << model->path << std::endl;
		std::cout << "\t No. of Meshes : " << model->meshes.size() << std::endl;
		std::cout << "\t Meshes:" << std::endl;
		int Meshindex{ 0 };
		for (auto mesh : model->meshes)
		{
			Material material = mesh->material;
			std::cout << "\t\t Material on Mesh #" << Meshindex++ << std::endl;
			log(material);
		}
	}
	void log(Material material)
	{
		int Textureindex = 0;
		std::cout << "\t\t\t Albedo :" << glm::to_string(material.Albedo) << std::endl;
		std::cout << "\t\t\t Metallic,Roughness,Emmisive : " << material.Metallic << "," << material.Metallic << "," << material.Emmisive << std::endl;
		std::cout << "\t\t\t AlphaType(OPAQUE,BLEND,MASK): " << (int)material.aplhaType << std::endl;
		std::cout << "\t\t\t No. of Textures: " << material.TexAlbedo.size() + material.TexEmmisive.size() + material.TexRoughMetal.size() + material.TexNormal.size() + material.TexOther.size() << std::endl;
		for (auto texture : material.TexAlbedo)
		{
			std::cout << "\t\t\t\t Texture #" << Textureindex++ << std::endl;
			std::cout << "\t\t\t\t\t Texture ID: " << texture->ID << std::endl;
			std::cout << "\t\t\t\t\t Texture path: " << texture->path << std::endl;
			std::cout << "\t\t\t\t\t Texture Type: Albedo" << std::endl;
		}
		for (auto texture : material.TexRoughMetal)
		{
			std::cout << "\t\t\t\t Texture #" << Textureindex++ << std::endl;
			std::cout << "\t\t\t\t\t Texture ID: " << texture->ID << std::endl;
			std::cout << "\t\t\t\t\t Texture path: " << texture->path << std::endl;
			std::cout << "\t\t\t\t\t Texture Type: TexRoughMetal" << std::endl;
		}
		for (auto texture : material.TexNormal)
		{
			std::cout << "\t\t\t\t Texture #" << Textureindex++ << std::endl;
			std::cout << "\t\t\t\t\t Texture ID: " << texture->ID << std::endl;
			std::cout << "\t\t\t\t\t Texture path: " << texture->path << std::endl;
			std::cout << "\t\t\t\t\t Texture Type: Normal" << std::endl;
		}
		for (auto texture : material.TexEmmisive)
		{
			std::cout << "\t\t\t\t Texture #" << Textureindex++ << std::endl;
			std::cout << "\t\t\t\t\t Texture ID: " << texture->ID << std::endl;
			std::cout << "\t\t\t\t\t Texture path: " << texture->path << std::endl;
			std::cout << "\t\t\t\t\t Texture Type: Emmisive" << std::endl;
		}
		for (auto texture : material.TexOther)
		{
			std::cout << "\t\t\t\t Texture #" << Textureindex++ << std::endl;
			std::cout << "\t\t\t\t\t Texture ID: " << texture->ID << std::endl;
			std::cout << "\t\t\t\t\t Texture path: " << texture->path << std::endl;
			std::cout << "\t\t\t\t\t Texture Type: Other" << std::endl;
		}
	}
};