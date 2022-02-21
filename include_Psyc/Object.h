#pragma once
#include<Model.h>
#include<Shader.h>
#include<string>
#include<vector>
class Object {
public:
	std::string name;
	Model* model;
	glm::mat4 transform;

	void draw(Shader ourShader);
	Object(glm::mat4 transform = glm::mat4(1.0f));
};