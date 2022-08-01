#version 460 core
const int MAX_CUBEMAPS = 5;
layout (triangles) in;
layout (triangle_strip, max_vertices=18*MAX_CUBEMAPS) out;

uniform mat4 shadowMatrices[6*MAX_CUBEMAPS];

out vec4 FragPos; // FragPos from GS (output per emitvertex)
flat out int cubemapFrag;

// The index of the cubemap to use in the cubemap array
uniform int numLights;
void main()
{
for(int layer = 0; layer <numLights; layer++)

        for(int face = 0; face < 6; face++)
        {
            
            for(int i = 0; i < 3; i++) // for each triangle's vertices
            {
                int faceLayer = layer * 6 +face;
                gl_Layer = faceLayer;
                FragPos = gl_in[i].gl_Position;
                cubemapFrag = faceLayer;
                gl_Position = shadowMatrices[faceLayer] * FragPos;
                EmitVertex();// Output variables are invalidated on each EmitVertex call, so reassign each time
            }    
            EndPrimitive();
        }
    
} 