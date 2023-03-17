#pragma once
#include <RMaterialBuilder.h>
#include <RMesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <assimp/scene.h>
#include <glad/glad.h>
class RMeshBuilder {
public:
	RMesh build(aiMesh* mesh, const aiScene* scene)
	{
		LoadVerticeData(mesh);
		UploadVerticeData();
		LoadMaterialData(mesh,scene);
		return rmesh;
	}
private:
    RMesh rmesh;
    RMaterialBuilder rMaterialBuilder;
	void LoadVerticeData(aiMesh* mesh)
	{
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;

                vertex.TexCoords = vec;
            }
            if (mesh->HasTangentsAndBitangents())
            {
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            rmesh.vertices.push_back(vertex);

        }
        // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                rmesh.indices.push_back(face.mIndices[j]);
        }

	}
    void UploadVerticeData()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &rmesh.VAO);
        glGenBuffers(1, &rmesh.VBO);
        glGenBuffers(1, &rmesh.EBO);

        glBindVertexArray(rmesh.VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, rmesh.VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, rmesh.vertices.size() * sizeof(Vertex), &rmesh.vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rmesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, rmesh.indices.size() * sizeof(unsigned int), &rmesh.indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
    void LoadMaterialData(aiMesh* mesh,const aiScene* scene)
	{
        rmesh.material = rMaterialBuilder.build(scene->mMaterials[mesh->mMaterialIndex]);
	}
	
};