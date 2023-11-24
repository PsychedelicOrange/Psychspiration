#pragma once
#include<RTexture.h>
#include<Material.h>
#include<vector>
#include <glm/glm.hpp>
using std::vector;
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec2 TexCoords;
    glm::vec2 TexCoords2;
};
class RMesh
{
public:
    vector<Vertex> vertices;
    vector<int>indices;
    Material material; 
    unsigned int VAO, EBO, VBO;
    glm::mat4* transform;
    // renderer
    int VBObufferid, VBOoffset,baseVertex;
};