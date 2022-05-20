#pragma once
#include <Mesh.h>
#include <TextureManager.h>
#include <Shader.h>
#include <Texture.h>
#include <assimp/scene.h>
#include <string>
#include <vector>
#include <bullet/btBulletDynamicsCommon.h>

class Model
{
public:
    //engine 
    static TextureManager textureManager;
    int instanceCount;
    int instanceOffset;
    // model data
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    std::string format;
    bool gammaCorrection;
    //physics
    btRigidBody* rigidBody;
    // constructor, expects a filepath to the 3D model.
    Model(std::string const& path, unsigned int* pbo = nullptr, bool gamma = false);
    Model();
    const aiScene* getpath(std::string path);
    void Draw(Shader& shader);
    void DrawInstanced(Shader& shader);
    void DrawShadowInstanced(Shader& shader);
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    const aiScene* loadModel(const std::string& path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    std::vector<Texture> loadMaterialTexturesEmbedded(aiMaterial* mat, const aiScene* scene, aiTextureType type, std::string typeName);
};