#version 430 core
layout (location = 0) in vec3 aPos;
layout(std430, binding = 1) buffer modelMatrices
{
    mat4 models[];
};

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform int instanceOffset;

void main()
{
    int instantIndex = instanceOffset + gl_InstanceID;
    gl_Position = lightSpaceMatrix * models[instantIndex] * vec4(aPos, 1.0);
}  