#pragma once
#include <EventHandler.h>
#include <ModelManager.h>
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
    ModelManager* modelManager;
    glm::mat4* instancedTransforms; // huge ass buffer

    std::string sceneName; // name of folder containing scene
    std::vector<Object*> objects; // vector of object pointers 
    std::vector<Model*> uniqueModels; // vector of unique model
    
    //lights
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList; // vector of lights 
    unsigned int numLights;
    // physics 
    Physics* physics;
    void setScale(float scale);
    Scene(std::string sceneName,Physics* physics,EventHandler* eventHandler, ModelManager* modelManager);
    Scene();
    //void draw(Shader ourShader);
    void setUpEvents(EventHandler* eventHandler);
    
    void populateObjects();
    void getInstanceCount();
    void makeHAB();
    void loadObjects();
    void drawObjects(Shader ourShader);
    void drawObjectsInstanced(Shader ourShader);
    void drawShadowObjectsInstanced(Shader ourShader);

    void populateHulls();
    int find(std::string t);
    void drawHulls(Shader ourShader);
    void setPhysics();
    void updatePhysics();
    void printdetail();
    glm::mat4 getmat4_csv(int i);
};