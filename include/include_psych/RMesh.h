#pragma once
#include<RTexture.h>
#include<Material.h>
#include<vector>
#include <glm/glm.hpp>
using std::vector;
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
class RMesh
{
    
public:
    vector<Vertex> vertices;
    vector<unsigned int>indices;
    Material material;
    unsigned int VAO, EBO, VBO;
};