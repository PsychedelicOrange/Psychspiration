#include <Object.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
void Object::draw(Shader ourShader)
{
    ourShader.use();
    //this->transform = glm::inverse(this->transform);
    //ourShader.setMat4("model", this->transform);
    ourShader.setMat4("model", (this->transform));
    this->model->Draw(ourShader);
}
void Object::drawHulls(Shader ourShader)
{
    ourShader.use();
    
    for (int i = 0; i < hulls.size(); i++)
    {
        ourShader.setMat4("model", (this->transform)*(this->hulls[i].transform) );// * this->transform);
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
    std::cout << "\n Hulls:\n No of Hulls: " << this->hulls.size() << "\n";

    for (int i = 0; i < hulls.size(); i++)
    {
        std::cout <<"hull: "<<i<<":\n " <<glm::to_string(this->hulls[i].transform);
    }
}
