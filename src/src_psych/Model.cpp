#include <Model.h>
#include <FileIO.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <iostream>
TextureManager Model::textureManager{};
Model::Model(std::string const& path,unsigned int* pbo, bool gamma ) : gammaCorrection(gamma)
{
    instanceCurr = -1;
    loadModel(path);
}
Model::Model()
{
    instanceCurr = -1;
}
const aiScene* Model::getpath(std::string path)
{
    //std::cout << "inside getpath()" << std::endl;
    return loadModel(path);
}
// draws the model, and thus all its meshes
void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}
void Model::DrawInstanced(Shader& shader)
{
    /*shader.use();
    shader.setInt("instanceOffset", this->instanceOffset);*/
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].DrawInstanced(shader,instanceCount);
}
void Model::DrawShadowInstanced(Shader& shader)
{
    /*shader.use();
    shader.setInt("instanceOffset", this->instanceOffset);*/
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].DrawShadowInstanced(shader, instanceCount);
}
const aiScene* Model::loadModel(const std::string& path)
{
    //std::cout << "inside loadmodel()\t no of meshes: " << std::endl;
    // read file via ASSIMP
    //std::string relativePath = getRelativePath();
    Assimp::Importer importer;
    //std::cout << relativePath + path;
    const aiScene* scene = importer.ReadFile(pathResource+ path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return scene;
    }
    // retrieve the directory path of the filepath
    directory = relativePath + path.substr(0, path.find_last_of('\\'));
    format = path.substr(path.find_last_of('.'), path.length());
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
    return scene;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // std::cout << "(inside processMesh) " << std::endl;
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // walk through each of the mesh's vertices
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

        vertices.push_back(vertex);

    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process aabb
    aiAABB thisaaBB = mesh->mAABB;
    mMax = { thisaaBB.mMax.x,thisaaBB.mMax.y,thisaaBB.mMax.y };
    mMin = { thisaaBB.mMin.x,thisaaBB.mMin.y,thisaaBB.mMin.y };
    /*
    for (int i = 0; i < vertices.size(); i++)
    {
        std::cout << "For Vertex " << i;
        std::cout <<"-Position :" << vertices[i].Position.x <<" " << vertices[i].Position.y <<" " << vertices[i].Position.z;
        std::cout << "normals :"<<vertices[i].Normal.x << " " << vertices[i].Normal.y << " " << vertices[i].Normal.z ;
        std::cout << "texcoords :"<<vertices[i].TexCoords.x << " " << vertices[i].TexCoords.y ;
        std::cout << "Tangents :" << vertices[i].Tangent.x << " " << vertices[i].Tangent.y << " " << vertices[i].Tangent.z;
        std::cout << "biTangents :" << vertices[i].Bitangent.x << " " << vertices[i].Bitangent.y << " " << vertices[i].Bitangent.z << std::endl;
    }
    */
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    if (this->format == ".obj" || this->format == ".gltf")
    {

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        std::vector<Texture> roughmetalMaps = loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_roughmetal");
        textures.insert(textures.end(), roughmetalMaps.begin(), roughmetalMaps.end());

    }
    //
    if (this->format == ".glb")
    {

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // 5. roughness + metalness maps
        std::vector<Texture> roughmetalMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_UNKNOWN, "texture_roughmetal");
        textures.insert(textures.end(), roughmetalMaps.begin(), roughmetalMaps.end());
        // return a mesh object created from the extracted mesh data

    }
    
    Mesh temp_mesh = Mesh(vertices, indices, textures);
    temp_mesh.vertices_flat = new float[vertices.size() * 3];
    for (int i = 0; i < vertices.size(); i++)
    {
        temp_mesh.vertices_flat[(i * 3) + 0] = vertices[i].Position.x;
        temp_mesh.vertices_flat[(i * 3) + 1] = vertices[i].Position.y;
        temp_mesh.vertices_flat[(i * 3) + 2] = vertices[i].Position.z;
    }
    temp_mesh.indices_flat = new unsigned int[indices.size()];
    for (int i = 0; i < indices.size(); i++)
        temp_mesh.indices_flat[i] = indices[i];
    return temp_mesh;

}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        //Texture* t = new Texture (str.C_Str());
        Texture* t = textureManager.getTexture(str.C_Str());
        t->type=typeName;
        textures.push_back(*(t));
        //// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        //bool skip = false;
        //for (unsigned int j = 0; j < textures_loaded.size(); j++)
        //{
        //    if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
        //    {
        //        textures.push_back(textures_loaded[j]);
        //        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
        //        break;
        //    }
        //}
        //if (!skip)
        //{   // if texture hasn't been loaded already, load it
        //    Texture texture;
        //    texture.id = TextureFromFile(str.C_Str(), this->directory);
        //    texture.type = typeName;
        //    //std::cout << std::endl << "Type : " << texture.type;
        //    texture.path = str.C_Str();
        //    textures.push_back(texture);
        //    textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        //}
    }
    /*for (int i = 0; i < textures.size(); i++)
    {
        std::cout << i << "'st Texture id , path , type = ";
        std::cout << textures[i].id << " ";
        std::cout << textures[i].type << " ";
        std::cout << textures[i].path << std::endl;
    }*/
    return textures;
}



std::vector<Texture> Model::loadMaterialTexturesEmbedded(aiMaterial* mat, const aiScene* scene, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    Texture textured;
    int NrTexture = mat->GetTextureCount(type);
    //std::cout << NrTexture;
    if (NrTexture == 2)
    {
        NrTexture = 1;
    }


    //std::cout << "NrTexture for "<<typeName<<":" << NrTexture<<std::endl;
    for (unsigned int i = 0; i < (NrTexture); i++)
    {

        aiString texture_file;

        mat->Get(AI_MATKEY_TEXTURE(type, i), texture_file);

        if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {

            //Texture x(texture, typeName);
            //textures.push_back(x);
            textures.push_back(*textureManager.getTextureEmbedded(texture, typeName));
            //textured.id = TextureEmbedded(texture, typeName);
            ////textured.id = hi();
            //textured.type = typeName;
            ////std::cout << std::endl << "Type : " << textured.type;
            //textured.path = "embedded";
        }
        else {
            //regular file, check if it exists and read it
            std::cout << "cant recognize embedded texture";
        }

    }

   /* for (int i = 0; i < textures.size(); i++)
    {
        std::cout << i << "'st Texture id , path , type = ";
        std::cout << textures[i].id << " ";
        std::cout << textures[i].type << " ";
        std::cout << textures[i].path << std::endl;
    }*/


    return textures;
}

