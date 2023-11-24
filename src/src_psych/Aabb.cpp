#include <Aabb.h>
Frustum* Aabb::camFrustum;
Aabb::Aabb(const glm::vec3& min, const glm::vec3& max,glm::mat4 transform)
{
    this->min = min;
    this->max = max;
    coords[0] = max;
    coords[1] = glm::vec3(max.x, max.y, min.z);
    coords[2] = glm::vec3(max.x, min.y, min.z);
    coords[3] = glm::vec3(max.x, min.y, max.z);
    coords[4] = glm::vec3(min.x, max.y, max.z);
    coords[5] = glm::vec3(min.x, max.y, min.z);
    coords[6] = min;
    coords[7] = glm::vec3(min.x, min.y, max.z);
    getMinMaxFromCoords();
    applyTransform(transform);
}
void Aabb::applyTransform(glm::mat4 transform)
{
    /*for (int i = 0; i < 8; i++)
    {
        coords[i] = glm::vec4(coords[i], 1) * transform;
    }*/
    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ transform * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = transform[0] * extents.x;
    const glm::vec3 up = transform[1] * extents.y;
    const glm::vec3 forward = -transform[2] * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    //We not need to divise scale because it's based on the half extention of the AABB
    center = globalCenter;
    extents = glm::vec3(newIi, newIj, newIk);
}
void Aabb::getMinMaxFromCoords()
{
    min = coords[0]; max = coords[0];
    for (int i = 1; i < 8; i++)
    {
        if (coords[i].x > max.x)
            max.x = coords[i].x;
        if (coords[i].y > max.y)
            max.y = coords[i].y;
        if (coords[i].z > max.z)
            max.z = coords[i].z;

        if (coords[i].x < min.x)
            min.x = coords[i].x;
        if (coords[i].y < min.y)
            min.y = coords[i].y;
        if (coords[i].z < min.z)
            min.z = coords[i].z;
    }

    center = (max + min) * 0.5f;
    extents = glm::vec3(max.x - center.x, max.y - center.y, max.z - center.z);
}
bool Aabb::isOnOrForwardPlan(const Plane& plan)
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plan.normal.x) + extents.y * std::abs(plan.normal.y) +
        extents.z * std::abs(plan.normal.z);

    return -r <= plan.getSignedDistanceToPlan(center);
}
bool Aabb::isOnFrustum(glm::mat4 transform)
{
    return (isOnOrForwardPlan(camFrustum->leftFace) &&
        isOnOrForwardPlan(camFrustum->rightFace) &&
        isOnOrForwardPlan(camFrustum->topFace) &&
        isOnOrForwardPlan(camFrustum->bottomFace) &&
        isOnOrForwardPlan(camFrustum->nearFace) &&
        isOnOrForwardPlan(camFrustum->farFace));
};