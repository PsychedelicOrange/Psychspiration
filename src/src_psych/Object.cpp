#include <Object.h>
#include <glm/gtx/string_cast.hpp>
#include <filesystem>
#include <iostream>
Object::Object(std::string name)
{
    this->name = name;
    this->Aabbmodel = new Model();
}
Object::Object(std::string name, std::string path,ModelManager* modelManager,  glm::mat4 transform, bool dynamic)
{
    this->name = name;
    this->path = path;
    this->modelManager = modelManager;
    this->transform = transform;
    this->dynamic = dynamic;
    this->Aabbmodel = new Model();
}
Object::Object(std::string name, Model* model, glm::mat4 transform, bool dynamic)
{
    this->model = model;
    this->name = name;
    this->transform = transform;
    this->dynamic = dynamic;
    this->Aabbmodel = new Model();
}

void Object::load()
{
    //this->model = new Model("Resources/Models/" + this->path + ".gltf");
    //this->model->textureManager = textureManager;
    this->model = modelManager->getModel(this->path);
    //this->aabb = { glm::vec3(glm::vec4(this->model->mMin,1)*this->transform),glm::vec3(glm::vec4(this->model->mMax,1) * this->transform) };s
    glm::vec3 min = this->model->mMin;
    glm::vec3 max = this->model->mMax;
    this->aabb = new Aabb{min, max,this->transform};
}
void Object::loadHulls()
{
     for (auto hull_ : this->hulls)
     {
         hull_->model = modelManager->getHull(hull_->path );
     }
}
void Object::draw(Shader ourShader)
{
    ourShader.use();
    //this->transform = glm::inverse(this->transform);
    //ourShader.setMat4("model", this->transform);
    ourShader.setMat4("model", (this->transform));
    this->model->Draw(ourShader);
}
void Object::drawInstanced(Shader ourShader)
{
    ourShader.use();
    ourShader.setInt("instanceOffset",this->model->instanceOffset);
    //this->transform = glm::inverse(this->transform);
    //ourShader.setMat4("model", this->transform);
    //ourShader.setMat4("model", (this->transform));
    this->model->DrawInstanced(ourShader);
}
void Object::drawHulls(Shader ourShader)
{
    debug = false;
    ourShader.use();
    for (int i = 0; i < hulls.size(); i++)
    {
        //hulls[i]->transform[0][3] = +transform[0][3];
        //hulls[i]->transform[1][3] = +transform[1][3];
        //hulls[i]->transform[2][3] = +transform[2][3];
       // glm::mat4 totalTrans = glm::mat4(1.0f);
        //totalTrans = glm::scale(totalTrans, glm::vec3(transform[0][0], transform[1][1], transform[2][2]));

        //totalTrans = this->transform* totalTrans;
        ourShader.setMat4("model",this->transform);
        hulls[i]->model->Draw(ourShader);
    }
}

void Object::drawAabb(Shader ourShader)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < 8; i++)
    {
        Vertex vertice; vertice.Position = aabb->coords[i];
        vertices.push_back(vertice);
    }
    std::vector<Texture> texCoords;
    std::vector<unsigned int> indices = { 0,1,2,    0,2,3,  0,4,7,  0,7,3,  4,7,6,  4,5,6,  6,5,1,  1,2,6,  4,0,1,  4,5,1,  7,3,2,  2,6,7 };
    Mesh* aabbMesh = new Mesh(vertices, indices,texCoords);
    ourShader.setMat4("model", this->transform);
    aabbMesh->Draw(ourShader);
}

glm::mat4 Object::getTransform()
{
    return transform;
}
void Object::printobj()
{
    std::cout << "Object read: " << this->name << "\n Transform matrix:\t" << glm::to_string(this->transform)<<"\n";
    std::cout << " Model:\n   No of meshes: " << this->model->meshes.size() << "\n";
    for (int i=0; i < this->model->meshes.size(); i++)
    {
        std::cout<< " Mesh "<< i<<":\n No of Textures: "<<this->model->meshes[i].textures.size();
    }
    std::cout << "\n Hulls:\n No of Hulls: " << this->hulls.size() << "\n";

    for (int i = 0; i < hulls.size(); i++)
    {
        std::cout <<"hull: "<<i<<":\n " <<glm::to_string(this->hulls[i]->transform);
    }
}
