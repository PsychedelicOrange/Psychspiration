#version 330 core
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
out vec3 TangentLightPos[NR_POINT_LIGHTS];
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    

   // FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);


    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    // calculating B with T and N
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    // directly taking from assimp
    //vec3 B = normalize(normalMatrix * aBitangent);

    mat3 TBN = transpose(mat3(T, B, N)); 
    vec3 TangentLightPos[NR_POINT_LIGHTS];
    for(int i=0;i<NR_POINT_LIGHTS;i++)
    {
        TangentLightPos[i] = TBN * pointLights[i].position;
    }
    
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;
    Normal = N; 
    gl_Position = projection * view * vec4(FragPos, 1.0);
}