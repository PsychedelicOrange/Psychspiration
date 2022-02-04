#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughMetal;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;


struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
    sampler2D texture_height1;
    sampler2D texture_roughmetal1;
    float shininess;
}; 

uniform bool donormals;
uniform bool existnormals;
uniform Material material;

void main()
{
    bool normals= donormals && existnormals;
    if(normals)
    {
        gNormal = texture(material.texture_normal1,TexCoords).rgb;
        gNormal = gNormal*2.0 - 1.0;
        gNormal = normalize(TBN* gNormal);
    }
    else
    {
        gNormal = Normal;
    }
    gPosition = FragPos;
    gAlbedo.rgb = texture(material.texture_diffuse1,TexCoords).rgb;
    gRoughMetal.rgb = texture(material.texture_roughmetal1,TexCoords).rgb;
}
