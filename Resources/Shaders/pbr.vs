#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

#define NR_POINT_LIGHTS 1
struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out mat3 TBNinverse;
out vec4 FragPosLightSpace;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;
void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    Normal = N; 
     TBNinverse = transpose(mat3(T,B,N));
    TangentViewPos  = TBNinverse * viewPos; // correct
    TangentFragPos  = TBNinverse * FragPos; // correct
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
     gl_Position = projection * view * vec4(FragPos, 1.0);
}