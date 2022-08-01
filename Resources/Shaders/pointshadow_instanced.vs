#version 460 core
layout (location = 0) in vec3 aPos;
layout(std430, binding = 1) buffer modelMatrices
{
    mat4 models[];
};

uniform mat4 model;
uniform int instanceOffset;
void main()
{
       int instanceIndex = instanceOffset+gl_InstanceID;
       gl_Position = models[instanceIndex] * vec4(aPos, 1.0);
}  
