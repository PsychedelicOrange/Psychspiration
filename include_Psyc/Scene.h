#pragma once
#include <EventHandler.h>
#include <Model.h>
#include <Object.h>
#include<Shader.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <Physics.h>
struct PointLight {
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float power;
    float constant;
    float linear;
    float quadratic;
    float size;
    float use_shadows;
    PointLight() {}
    PointLight(std::string name, glm::vec3 position, float power, float size = 100.f, glm::vec3 color = glm::vec3(1.f),bool use_shadows = true)
    {
        this->name= name ;
        this->position = position;
        this->power = power;
        this->size = size;
        this->color = color;
        this->constant = this->linear = this->quadratic = 0;
        this->use_shadows = use_shadows;
    }
};
struct GPULight {
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

class Scene
{
//importer 
   
public:
    //engine
    EventHandler* eventHandler;
    //ResourceManager* resourceManager;

    std::string sceneName; // name of folder containing scene
    std::vector<Object*> objects; // vector of object pointers  
     //model transforms and info 
    std::vector<std::string> propvec;
    std::vector<std::string> name;
    std::vector<std::string> model_paths;
    std::vector<glm::mat4> transforms;
    //lights
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList; // vector of lights 
    unsigned int numLights;
    // physics 
    Physics* physics;
    void setScale(float scale);
    Scene(std::string sceneName,Physics* physics,EventHandler* eventHandler);
    Scene();
    //void draw(Shader ourShader);
    void setUpEvents(EventHandler* eventHandler);
    
    void populateObjects();
    void loadObjects();
    void drawObjects(Shader ourShader);

    void populateHulls();
    int find(std::string t);
    void drawHulls(Shader ourShader);
    void setPhysics();
    void updatePhysics();
    void printdetail();
    glm::mat4 getmat4_csv(int i);
};