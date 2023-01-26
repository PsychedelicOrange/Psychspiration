#pragma once
#include <Shader.h>
#include <Texture.h>
#include <vector>
#include <bullet/btBulletDynamicsCommon.h>
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
}; 
struct Material {
    glm::vec3 albedo = { 0.5,0.5,0.5 };
    float metallic = 0;
    float roughness = 0.5;
    float emmisive = 0;
    float alphaClip = 0.5;
    std::string alphaMode;

};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    Material material;
    unsigned int VAO;
    //physics 
    float* vertices_flat;
    unsigned int * indices_flat;
    btCollisionShape* colShape;
    bool texturesExist[4] = {0,0,0,0}; // diffuse,normal,roughmetal,emmisive
    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,Material material = Material());
    void setupMaterial();
    void Draw(Shader& shader);
    void DrawInstanced(Shader& shader,int instancedCount);
    void DrawShadowInstanced(Shader& shader, int instancedCount);
private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void setupTextures(Shader& shader);
    void draw(Shader& shader);
    void drawInstanced(Shader& shader,int instanceCount);
};

