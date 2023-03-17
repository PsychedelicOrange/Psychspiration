#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <FileIO.h>
#include <RModel.h>
#include <RMeshBuilder.h>

class ASSIMPimporter
{
    RModel* model;
    RMeshBuilder rMeshBuilder;
public:
	RModel* load(string path)
	{
        model = new RModel();
        model->path = path;
        Assimp::Importer importer;
        //std::cout << relativePath + path;
        const aiScene* scene = importer.ReadFile(pathResource + path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return nullptr; 
        }
        return processNode(scene->mRootNode, scene);
	}
    RModel* processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            model->meshes.push_back(rMeshBuilder.build(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

};
