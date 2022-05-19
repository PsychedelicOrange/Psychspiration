#include <Object.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
Object::Object(std::string name)
{
    this->name = name;
}
Object::Object(std::string name, std::string path,ModelManager* modelManager,  glm::mat4 transform, bool dynamic)
{
    this->name = name;
    this->path = path;
    this->modelManager = modelManager;
    this->transform = transform;
    this->dynamic = dynamic;
}
Object::Object(std::string name, Model* model, glm::mat4 transform, bool dynamic)
{
    this->model = model;
    this->name = name;
    this->transform = transform;
    this->dynamic = dynamic;
}

void Object::load()
{
    //this->model = new Model("Resources/Models/" + this->path + ".gltf");
    //this->model->textureManager = textureManager;
    this->model = modelManager->getModel(this->path);
}
void Object::loadHulls()
{
    for (int i = 0; i < hulls.size(); i++)
    {
        //this->hulls[i]->model = new Model("Resources/Models/"+this->hulls[i]->path + ".gltf");
        this->hulls[i]->model = modelManager->getModel(this->hulls[i]->path);
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
        ourShader.setMat4("model", (this->transform)*(this->hulls[i]->transform) );// * this->transform);
        hulls[i]->model->Draw(ourShader);
    }
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
