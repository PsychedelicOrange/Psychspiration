#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TBNinverse;

const float PI = 3.14159265359;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
    sampler2D texture_height1;
    sampler2D texture_roughmetal1;
    float shininess;
};
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
uniform samplerCubeArray depthMap;
uniform samplerCube skybox;

uniform float far_plane;
uniform int numLights;
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float ShadowCalculation(vec3 fragPos,int i);
float ShadowCalculationPCF(vec3 fragPos);
uniform bool doshadows;
uniform bool donormals;
uniform bool existnormals;
uniform Material material;
// array of offset direction for sampling (shadows)
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void main()
{

    vec3 albedo     = texture(material.texture_diffuse1,TexCoords).rgb;
    //vec3 albedo = {1,0,0};
    float metallic  = texture(material.texture_roughmetal1,TexCoords).b;
    float roughness = texture(material.texture_roughmetal1,TexCoords).g;
    roughness *= 0.6; 
    float ao =0.03f;
    vec3 ambient = albedo * ao;
    bool normals = (donormals && existnormals);
    vec3 N = vec3(0.0);
    vec3 V;
    vec3 fragPos;

     if(normals)
    {
        N = texture(material.texture_normal1,TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0); 
        V = normalize(TangentViewPos - TangentFragPos);
        fragPos = TangentFragPos;
    }
    else
    {
         N = normalize(Normal);
	     V = normalize(viewPos - FragPos);
         fragPos = FragPos;
    }

     vec3 F0 = vec3(0.04f);
     F0 = mix(F0,albedo,metallic);
	 
     vec3 Lt = vec3(0.0f);
	for(int i =0 ;i< numLights;++i)
	{
        vec3 Lo = vec3(0.0f);
        vec3 L ;
        
        float distance = length(gpuLight[i].position.xyz- fragPos);
       
        if(normals)
        {
            L = normalize (TBNinverse * gpuLight[i].position.xyz- fragPos); // make it offline bro so it fast
            distance = length(TBNinverse * gpuLight[i].position.xyz- fragPos);
        }
        else{
	        L = normalize (gpuLight[i].position.xyz- fragPos);
        }
        float attenuation = 1.0 / (distance * distance); // attenuation based on inverse square law
        //float attenuation =1.0;
        //float range = gpuLight[i].range;
        //if(distance<range)
        //{
        //    attenuation *= (range-distance)/range; //sphere falloff
        //}
        //else{
        //    continue;
        //}
		    vec3 H = normalize (V+L);
            vec3 radiance = gpuLight[i].intensity * gpuLight[i].color.xyz * attenuation * 0.1f;
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
            Lo = (kd * albedo / PI + specular) * radiance * (max(dot(N,L),0.0f));
            float shadow = doshadows ? ShadowCalculation(FragPos,i) : 1.0;   
            Lo *= shadow;
            Lt += Lo;
	}
    
    vec3 color   = ambient + Lt;
    //gamma correction done in glEnable sRGB
    //FragColor = vec4(gpuLight[1].position);
    FragColor = vec4(vec3(color),1.0);
    //FragColor = vec4(1,1,1,1.f);
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
float ShadowCalculation(vec3 fragPos,int i)
{   
    float bias = 0.05;
    float shadow = 0.0;

    shadow = 0;
    // get vector between fragment position and light position 
    vec3 fragToLight = fragPos - gpuLight[i].position.xyz; 
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, vec4(fragToLight,i)).r; 
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value 
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position 
    float currentDepth = length(fragToLight);
    // test for shadows 
     // we use a much larger bias since depth is now in [near_plane, far_plane] range 
    shadow = (closestDepth +  bias) < currentDepth ? 0.0 : 1.0;
    // display closestDepth as debug (to visualize depth cubemap) 
     //FragColor = vec4(,1.0); 
    //FragColor = vec4(shadow*vec3(1.0f), 1.0); 
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0); 
    return shadow;
}
float ShadowCalculationPCF(vec3 fragPos)
{
    float shadow = 0.0;
    float TotalShadow = 0.0;
    float bias = 0.05;
    int samples = 1;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 10000.0;
    for(int i=0;i<numLights;i++)
    {
    shadow = 0.0;
     // get vector between fragment position and light position 
    vec3 fragToLight = fragPos - gpuLight[i].position.xyz; 
    // now get current linear depth as the length between the fragment and light position 
    float currentDepth = length(fragToLight);
    
        for(int j = 0; j < samples; ++j)
        {
            float closestDepth = texture(depthMap, vec4(fragToLight + gridSamplingDisk[j] * diskRadius,i)).r;
            closestDepth *= far_plane;   // undo mapping [0;1]
            if(currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
        shadow /= float(samples);
        TotalShadow += shadow;
    }
    TotalShadow/=float(numLights);
    //clamp(TotalShadow,0,1);
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return TotalShadow;
}