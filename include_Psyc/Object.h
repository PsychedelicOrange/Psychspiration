#pragma once
#include<Model.h>
#include<ModelManager.h>
#include<Shader.h>
#include<string>
#include<vector>

struct hull {
	Model* model;
	std::string path;
	glm::mat4 transform;
	hull(std::string path, glm::mat4 transform)
	{
		this->path = path;
		this->transform = transform;
	}
	hull(Model* model, glm::mat4 transform)
	{
		this->model = model;
		this->transform = transform;
	}
};
class Object {
public:
	//engine
	ModelManager* modelManager;
	
	//meta
	std::string name;
	bool debug;
	
	//graphics
	std::string path;
	Model* model;
	glm::mat4 transform;
	Aabb aabb;
	//physics
	bool dynamic;
	std::vector<hull*> hulls;
	
	void load();
	void loadHulls();
	void setPhysics();
	
	void draw(Shader ourShader);
	void drawInstanced(Shader ourShader);
	void drawHulls(Shader ourShader);

	
	void printobj();

	Object(std::string name);
	Object(std::string name, std::string path,ModelManager* modelManager, glm::mat4 transform = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1)), bool dynamic = 0);
	Object(std::string name, Model* model, glm::mat4 transform = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(0, 0, 0, 1)), bool dynamic =0);
	glm::mat4 getTransform();
};
