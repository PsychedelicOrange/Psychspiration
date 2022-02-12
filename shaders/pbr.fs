#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TBNinverse;
layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
    uvec2 screenDimensions;
    float scale;
    float bias;
};

const float PI = 3.14159265359;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_specular1;
    sampler2D texture_height1;
    sampler2D texture_roughmetal1;
    float shininess;
}; 
struct PointLight {
    vec4 position;
    vec4 color;
    bool enabled;
    float intensity;
    float range;
};
struct LightGrid{
    uint offset;
    uint count;
};
layout (std430, binding = 2) buffer screenToView{
    mat4 inverseProjection;
    uvec4 tileSizes;
    uvec2 screenDimensions;
    float scale;
    float bias;
};
layout (std430, binding = 3) buffer lightSSBO{
    PointLight pointLight[];
};
layout (std430, binding = 4) buffer lightIndexSSBO{
    uint globalLightIndexList[];
};
layout (std430, binding = 5) buffer lightGridSSBO{
    LightGrid lightGrid[];
};
uniform vec3 viewPos;
uniform float albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
uniform samplerCube depthMap;
uniform float far_plane;
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float ShadowCalculation(vec3 fragPos);
uniform bool doshadows;
uniform bool donormals;
uniform bool existnormals;
uniform Material material;

void main()
{
    vec3 albedo     = texture(material.texture_diffuse1,TexCoords).rgb;
    float metallic  = texture(material.texture_roughmetal1,TexCoords).b;
    float roughness = texture(material.texture_roughmetal1,TexCoords).g;
    float ao =1.0f;

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

     //Locating which cluster you are a part of
    uint zTile     = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0.0));
    uvec3 tiles    = uvec3( uvec2( gl_FragCoord.xy / tileSizes[3] ), zTile);
    uint tileIndex = tiles.x +
                     tileSizes.x * tiles.y +
                     (tileSizes.x * tileSizes.y) * tiles.z;  
    uint lightCount       = lightGrid[tileIndex].count;
    uint lightIndexOffset = lightGrid[tileIndex].offset;

	vec3 Lo = vec3(0.0f);
	for(int i =0 ;i< lightCount;++i)
	{
        vec3 L ;
        float distance;
        if(normals)
        {
            L = normalize (TBNinverse * pointLight[i].position- fragPos);
            distance = length(TBNinverse * pointLight[i].position- fragPos);
        }
        else{
	        L = normalize (pointLight[i].position- fragPos);
            distance = length(pointLight[i].position- fragPos);
        }
		    vec3 H = normalize (V+L);
		    float attenuation = 1.0 / (distance * distance);
            float radius = pointLight[i].range;
            float attenuation = pow(clamp(1 - pow((distance / radius), 4.0), 0.0, 1.0), 2.0)/(1.0  + (distance * distance) );
            vec3 radiance = pointLight[i].color * attenuation;
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
    vec3 ambient = vec3(0.5) * albedo * ao;
    float shadow = doshadows ? ShadowCalculation(FragPos) : 1.0; 
    vec3 color   = ambient + shadow * Lo; 
    //gamma correction done in glEnable sRGB
    //FragColor = vec4(color,1.0f);
    FragColor = vec4(vec3(0.1,1,0.1),1.0);
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
float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position 
    vec3 fragToLight = fragPos - pointLights[0].position; 
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r; 
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value 
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position 
    float currentDepth = length(fragToLight);
    // test for shadows 
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range 
    float shadow = (closestDepth +  bias) < currentDepth ? 0.0 : 1.0;
    // display closestDepth as debug (to visualize depth cubemap) 
     //FragColor = vec4(,1.0); 
    //FragColor = vec4(shadow*vec3(1.0f), 1.0); 
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0); 
    return shadow;
}

float linearDepth(float depthSample){
    float depthRange = 2.0 * depthSample - 1.0;
    // Near... Far... wherever you are...
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}