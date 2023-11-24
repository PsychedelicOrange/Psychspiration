#version 460 core
in vec4 FragPos;
flat in int cubemapFrag;
struct GPULight {
    vec4 position;
    vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};
layout (std140, binding = 3) uniform lightUBO{
    GPULight gpuLight[100];//max lights do later using shader io before init
};
uniform float far_plane;
uniform int numLights;
//uniform int iLight;
void main()
{
  //for(int i =0;i<numLights;i++)
  //{
        float lightDistance = length(FragPos.xyz - gpuLight[cubemapFrag/6].position.xyz);
        // map to [0;1] range by dividing by far_plane
        lightDistance = lightDistance / far_plane;
        // write this as modified depth
        gl_FragDepth = lightDistance ;
   //} 
}