#include <Object.h>
#include <iostream>
void Object::draw(Shader ourShader)
{
    ourShader.use();
    ourShader.setMat4("model", this->transform);
    this->model->Draw(ourShader);
}
Object::Object(glm::mat4 transform)
{
    this->transform = transform;
}