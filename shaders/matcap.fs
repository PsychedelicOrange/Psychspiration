#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;


const float PI = 3.14159265359;


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
uniform vec3 viewPos;
uniform float albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
uniform float far_plane;
uniform int numLights;
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float ShadowCalculation(vec3 fragPos);



void main()
{
    
    vec3 ambient = albedo * ao;
    bool normals = (donormals && existnormals);
    vec3 N;
    vec3 V;
    vec3 fragPos;
    
    N = normalize(Normal);
	V = normalize(viewPos - FragPos);
    fragPos = FragPos;
    
     vec3 F0 = vec3(0.04f);
     F0 = mix(F0,albedo,metallic);
	vec3 Lo = vec3(0.0f);
	for(int i =0 ;i< numLights;++i)
	{
        vec3 L ;
        
        float distance = length(gpuLight[i].position.xyz- fragPos);
       
       
	        L = normalize (gpuLight[i].position.xyz- fragPos);
        
        float attenuation = 1.0 / (distance * distance); // attenuation based on inverse square law
        
        
		    vec3 H = normalize (V+L);
            vec3 radiance = gpuLight[i].intensity*gpuLight[i].color.xyz * attenuation*0.1f;
            //vec3 radiance = vec3(23.47, 21.31, 20.79) * attenuation;

		    // calculate Cook-Terrence specular BRDF 
		        // F
		        vec3 F = fresnelSchlick(max(dot(H,V),0.0f),F0);
                // D
                float NDF = DistributionGGX(N, H, roughness); 
                // G
                float G   = GeometrySmith(N, V, L, roughness);       
                vec3 specular = (NDF * G * F) / (4*max(dot(N,V),0.0f)*max(dot(N,L),0.0f)+0.0001);

            vec3 ks = F;
            vec3 kd = vec3(1.0)- ks;
            kd *= (1.0-metallic);
            Lo += (kd * albedo / PI + specular) * radiance * (max(dot(N,L),0.0f));
            
	}

    vec3 color   = ambient + (1.0) * Lo; 
    //gamma correction done in glEnable sRGB
    //FragColor = vec4(gpuLight[1].position);
    FragColor = vec4(vec3(color),1.0);
    //FragColor = vec4(pointLights[0].position,0.0f);
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
