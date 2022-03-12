#pragma once
#include<Model.h>
#include<Shader.h>
#include<string>
#include<vector>
struct hull {
	Model* model;
	glm::mat4 transform;
	hull(Model* model, glm::mat4 transform)
	{
		this->model = model;
		this->transform = transform;
	}
};
class Object {
public:
	//meta
	std::string name;
	//graphics
	Model* model;
	glm::mat4 transform;
	//physics
	bool dynamic=0;
	std::vector<hull> hulls;
	void printobj();
	void draw(Shader ourShader);
	void drawHulls(Shader ourShader);
	bool debug;
	Object(std::string name);
	Object(std::string name, Model* model, glm::mat4 transform = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1)));
	glm::mat4 getTransform();
};