#pragma once
#include <Model.h>
#include<Shader.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>


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
    std::vector<std::string> propvec;
    std::vector<std::string> name;
    std::vector<glm::vec3> location;
    std::vector<glm::vec3> scale;
    std::vector<glm::vec4> rotation;
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList;
    std::vector<Model*> models;
    unsigned int numLights;
    //std::vector<glm::vec4> rotation_axis;
    std::string sceneName;
    //Model* models;
    Scene(std::string sceneName);
    void draw(Shader ourShader);
    Model* load();
};