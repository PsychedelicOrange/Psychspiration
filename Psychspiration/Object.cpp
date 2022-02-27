#include <Object.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
void Object::draw(Shader ourShader)
{
    ourShader.use();
    ourShader.setMat4("model", this->transform);
    this->model->Draw(ourShader);
}
void Object::drawHulls(Shader ourShader)
{
    ourShader.use();
    ourShader.setMat4("model", this->transform);
    for (int i = 0; i < hulls.size(); i++)
    {
        hulls[i].model->Draw(ourShader);
    }
}
Object::Object(std::string name)
{   
    this->name = name;
}
Object::Object(std::string name,Model* model,glm::mat4 transform)
{
    this->model = model;
    this->name = name;
    this->transform = transform;
    
}
void Object::printobj()
{
    std::cout << "Object read: " << this->name << "\n Transform matrix:\t" << glm::to_string(this->transform)<<"\n";
    std::cout << " Model:\n   No of meshes: " << this->model->meshes.size() << "\n";
    for (int i=0; i < this->model->meshes.size(); i++)
    {
        std::cout<< " Mesh "<< i<<":\n   No of Textures: "<<this->model->meshes[i].textures.size();
    }
}
