#include <Mesh.h>
#include <vector>
#include <glad/glad.h>
#include <bullet/btBulletDynamicsCommon.h>

// constructor
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->vertices_flat = vertices_flat;
    this->indices_flat = indices_flat;
    this->material = material;
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
    setupMaterial();
}
void Mesh::setupMaterial()
{
    std::cout << textures.size();
    for (unsigned int i = 0; i < (textures.size()); i++)
    {
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
        {
            texturesExist[0] = 1;
        }
        else if (name == "texture_normal")
        {
            texturesExist[1] = 1;
        }
        else if (name == "texture_roughmetal")
        {
            texturesExist[2] = 1;
        }
    }
}
void Mesh::Draw(Shader& shader)
{
    setupTextures(shader);
    draw(shader);
}
void Mesh::DrawInstanced(Shader& shader,int instanceCount)
{
    setupTextures(shader);
    drawInstanced(shader,instanceCount);
}
void Mesh::DrawShadowInstanced(Shader& shader, int instanceCount)
{
    //setupTextures(shader);
    drawInstanced(shader, instanceCount);
}
void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

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
void Mesh::setupTextures(Shader& shader)
{

    for (unsigned int i = 0; i < (textures.size()); i++)
    { 
        std::string name = (textures[i].type);
        shader.setInt(name.c_str(), i);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    shader.setBool("existnormals", texturesExist[1]);
    shader.setBool("existdiffuse", texturesExist[0]);
    shader.setBool("existroughmetal", texturesExist[2]);
    shader.setVec3("u_albedo", material.albedo);
    shader.setFloat("u_metallic", material.metallic);//material->metallic
    shader.setFloat("u_roughness", material.roughness);//material->roughness

}
void Mesh::draw(Shader& shader)
{
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
void Mesh::drawInstanced(Shader& shader,int instanceCount)
{
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,instanceCount);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}