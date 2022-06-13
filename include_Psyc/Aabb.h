#pragma once
#include <glm/glm.hpp>
struct Plane {
    glm::vec3 normal = { 0,1,0 };
    float distance = 0;
};
class Aabb {
public :
    glm::vec3 center{ 0.f, 0.f, 0.f };
    glm::vec3 extents{ 0.f, 0.f, 0.f };
    Aabb(const glm::vec3& min, const glm::vec3& max)
    {
        center = (max + min) * 0.5f;
        extents = glm::vec3( max.x - center.x, max.y - center.y, max.z - center.z );
    }

};