#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TBNinverse;
in vec4 FragPosLightSpace;
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
    GPULight gpuLight[100]; //max lights do later using shader io before init
};
uniform vec3 viewPos;
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float ao;
uniform samplerCubeArray depthMap;
uniform sampler2D depthMap_dir;
uniform sampler2D   brdfLUT;  
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughmetal;
uniform float far_plane;
uniform int numLights;
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float ShadowCalculation(vec3 fragPos,int i);
float ShadowCalculation_dir(vec3 N);
float ShadowCalculationPCF(vec3 fragPos,int i);
uniform vec3 dir_direction;
uniform vec3 dir_color;
uniform float dir_intensity;
uniform bool doshadows;
uniform bool donormals;
uniform bool existnormals;
uniform bool existdiffuse;
uniform bool existroughmetal;

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
    vec3 albedo = u_albedo; float metallic = u_metallic,roughness = u_roughness;
    if(existdiffuse)
    {
        vec4 texcolor = texture(texture_diffuse,TexCoords).rgba;
        albedo     = pow(texcolor.rgb,vec3(2.2));
    }
    if(existroughmetal)
    {
        metallic  = texture(texture_roughmetal,TexCoords).b;
        roughness = texture(texture_roughmetal,TexCoords).g;
    }
    bool normals = (donormals && existnormals);
    //else block
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 fragPos = FragPos;
    if(normals)
    {
        //N = pow(texture(texture_normal1,TexCoords).rgb,vec3(2.2));
        N = texture(texture_normal,TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0); 
        V = normalize(TangentViewPos - TangentFragPos);
        fragPos = TangentFragPos;
    }
     vec3 F0 = vec3(0.04f);
     F0 = mix(F0,albedo,metallic);
	 
     vec3 Lt = vec3(0.0f);
     // point lights 
	for(int i =0 ;i< numLights;++i)
	{
        vec3 Lo = vec3(0.0f);
        vec3 L ;
        float distance = length(gpuLight[i].position.xyz- fragPos);
        if(distance > gpuLight[i].range)
        {
        continue;
        }
        if(normals)
        {
            L = normalize (TBNinverse * gpuLight[i].position.xyz- fragPos); // make it offline bro so it fast
            distance = length(TBNinverse * gpuLight[i].position.xyz- fragPos);
        }
        else{
	        L = normalize (gpuLight[i].position.xyz- fragPos);
        }
		    vec3 H = normalize (V+L);
            float d = gpuLight[i].range;
            float intensity = gpuLight[i].intensity * (d-distance)/(d*pow(distance,2)) ;//* 0.1f;
            vec3 radiance = intensity * gpuLight[i].color.xyz;//* 0.1f;
		    // calculate Cook-Terrence specular BRDF 
		        // F
		        vec3 F = fresnelSchlick(max(dot(H,V),0.0f),F0);
                // D
                float NDF = DistributionGGX(N, H, roughness); 
                // G
                float G   = GeometrySmith(N, V, L, roughness);       
                vec3 numerator    = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
                vec3 specular = numerator / denominator;

                //vec3 specular = (NDF * G * F) / (4*max(dot(N,V),0.0f)*max(dot(N,L),0.0f)+0.0001);

            vec3 ks = F;
            vec3 kd = vec3(1.0)- ks;
            kd *= (1.0-metallic);
            Lo = (kd * albedo / PI + specular) * radiance * (max(dot(N,L),0.0f));
            float shadow = doshadows ? ShadowCalculation(FragPos,i) : 1.0;   
            Lo *= shadow;
            Lt += Lo;
	}
    // directional light
    vec3 L;
    if(normals)
        L = normalize(TBNinverse* dir_direction);
    else
        L = normalize(dir_direction);
    vec3 H = normalize (V+L);
    vec3 radiance = dir_intensity * dir_color.xyz * 0.1;
   // radiance = vec3(1000);
	vec3 F = fresnelSchlick(max(dot(H,V),0.0f),F0);
    float NDF = DistributionGGX(N, H, roughness); 
    float G   = GeometrySmith(N, V, L, roughness);       
    vec3 specular = (NDF * G * F) / (4*max(dot(N,V),0.0f)*max(dot(N,L),0.0f)+0.0001);
    vec3 ks = F;
    vec3 kd = vec3(1.0)- ks;
    kd *= (1.0-metallic);
    vec3 Lo = (kd * albedo / PI + specular) * radiance * (max(dot(N,L),0.0f));
    float shadow = doshadows ? ShadowCalculation_dir(N):1.0; 
    Lo*= shadow;
    //Lt+= Lo; 
    // ambient IBL
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(-V, N); 
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 amb_specular = prefilteredColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + amb_specular) ;//* ao;
    //float diffuse = max(dot(N,L));
    //ambient = vec3(0);
    vec3 color = ambient + Lt ;
    //color = ambient ;
    //gamma correction done in glEnable sRGB
    //FragColor = vec4(gpuLight[1].position);
    FragColor = vec4(vec3(color),(1.0f));
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
float ShadowCalculation_dir(vec3 N)
{
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap_dir, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    //vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(dir_direction);
    float bias = max(0.05 * (1.0 - dot(N, lightDir)), 0.001);
    bias = 0.0000;
    //float bias = 0.005;
    // check whether current frag pos is in shadow
     //float shadow = (currentDepth - bias) > closestDepth  ? 1.0 : 0.0;
    // PCF
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap_dir, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap_dir, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 1.0;
     //FragColor = vec4(vec3(closestDepth),0);
    //float shadow = currentDepth > closestDepth ? 1.0 :0.0;
    //shadow = closestDepth;
    return 1-shadow;
}
float ShadowCalculationPCF(vec3 fragPos,int i)
{
    float shadow = 0.0;
    float TotalShadow = 0.0;
    float bias = 0.05;
    int samples = 10;
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
    //TotalShadow/=float(numLights);
    //clamp(TotalShadow,0,1);
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return TotalShadow;
}