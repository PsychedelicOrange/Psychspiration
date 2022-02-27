#pragma once
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
public:
    std::string sceneName; // name of folder containing scene
    std::vector<Object*> objects; // vector of object pointers assigned in scene constructor 
    //model transforms and info 
    std::vector<std::string> propvec;
    std::vector<std::string> name;
    std::vector<glm::mat4> transforms;
    //lights
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList;
    unsigned int numLights;
    // physics 
    Physics physics;
  
    Scene(std::string sceneName);
    //void draw(Shader ourShader);
    void drawobj(Shader ourShader);
    void cleanupModels();
    void loadModels();
    void loadPhysics();
    void loadHulls();
    int find(std::string t);
    void drawHulls(Shader ourShader);
    void updatePhysics();
    void printdetail();
    glm::mat4 getmat4_csv(int i);
};