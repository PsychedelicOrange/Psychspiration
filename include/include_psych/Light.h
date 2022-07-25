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
    PointLight() {
        name = "default";
        position = glm::vec3(0, 1, 0);
        power = 100;
        size = 100.f;
        color = glm::vec3(1.f);
        use_shadows = true;
    }
    PointLight(std::string name, glm::vec3 position, float power, float size = 100.f, glm::vec3 color = glm::vec3(1.f), bool use_shadows = true)
    {
        this->name = name;
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
struct DirectionalLight {
    DirectionalLight() {
        direction = { -20.0f, 40.0f, -10.0f }; color = { 1,1,1 }; intensity = 100.0f;
    }
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
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