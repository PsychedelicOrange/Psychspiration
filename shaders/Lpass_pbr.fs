#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

#define NR_POINT_LIGHTS 1
const float PI = 3.14159265359;

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gRoughMetal;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
uniform bool doshadows;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 albedo = texture(gAlbedo, TexCoords).rgb;
    float metallic = texture(gRoughMetal, TexCoords).b;
    float roughness = texture(gRoughMetal, TexCoords).g;
    float ao =1.0f;

    vec3 V = normalize(viewPos - FragPos);
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0,albedo,metallic);

	vec3 Lo = vec3(0.0f);
	for(int i =0 ;i< NR_POINT_LIGHTS;++i)
	{
    vec3 L ;
    float distance;
   
	L = normalize (pointLights[i].position- FragPos);
    distance = length(pointLights[i].position- FragPos);
    
	vec3 H = normalize (V+L);
	float attenuation = 1.0 / (distance * distance);
    vec3 radiance = pointLights[i].color * attenuation;
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
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color   = ambient + Lo; 
    //gamma correction done in glEnable sRGB
    //or not 
    float gamma = 2.2;
    FragColor.rgb = pow(color.rgb, vec3(1.0/gamma));

    //FragColor = vec4(color,1.0f);
    //FragColor = vec4(vec3(N),0.0f);
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
