#pragma once
#include <glm/glm.hpp>
#include <string>
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
struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
};
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float angleInnerCone;
    float angleOuterCone;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;
};