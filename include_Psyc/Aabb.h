#pragma once
#include <glm/glm.hpp>
#include <Frustum.h>
class Aabb {
public :
    glm::vec3 coords[8];
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center{ 0.f, 0.f, 0.f };
    glm::vec3 extents{ 0.f, 0.f, 0.f };
    static Frustum* camFrustum;
    Aabb(){}
    Aabb(const glm::vec3& min, const glm::vec3& max);
    void applyTransform(glm::mat4 transform);
    void getMinMaxFromCoords();
    Aabb(const glm::vec3& min, const glm::vec3& max, glm::mat4 transform);
    bool isOnOrForwardPlan(const Plane& plan);
    bool isOnFrustum(glm::mat4 transform);
};