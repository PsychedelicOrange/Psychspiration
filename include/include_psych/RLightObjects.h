#include <glm/glm.hpp>
struct GPUpLight {
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
}; // look up the padding issues for a ubo