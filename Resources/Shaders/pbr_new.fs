#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TBNinverse;

//Declarations
    //constants
const float PI = 3.14159265359;
    //functions
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 srgbEncode(vec3 color);
    //structs
struct GPULight {
    vec4 position;
    vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

//uniforms
    //material values
uniform vec3 u_albedo;
uniform vec3 u_emmisive;
uniform float u_metallic;
uniform float u_roughness;
    //textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughmetal;
uniform sampler2D texture_emmisive;
    //bools and numbers
uniform bool existdiffuse;
uniform bool existnormals;
uniform bool existroughmetal;
uniform bool existemmisive;

uniform bool existOpacity;

uniform vec3 viewPos;
uniform int numPLights;
    //buffers
layout (std140, binding = 3) uniform lightUBO{
    GPULight gpuLight[100]; //max lights do later using shader io before init
};

void main()
{
    float gamma = 2.2;
    vec3 albedo = u_albedo;
    float metallic = u_metallic,roughness = u_roughness;
    vec4 texcolor;
    if(existdiffuse)
    {
        texcolor = texture(texture_diffuse,TexCoords).rgba;
        if(texcolor.a < 0.1)
            discard;
        albedo = texcolor.rgb;
        albedo = pow(albedo,vec3(gamma));
    }
    if(existroughmetal)
    {
        metallic  = texture(texture_roughmetal,TexCoords).b;
        roughness = texture(texture_roughmetal,TexCoords).g;
    }

    vec3 N = vec3(0.0);
    vec3 V;
    vec3 fragPos;

    if(existnormals)
    {
        N = texture(texture_normal,TexCoords).rgb;
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
    vec3 F0 = vec3(0.25);
    F0 = mix(F0,albedo,metallic) ;
    //if(metallic == 1)F0 = albedo;
    //if(metallic == 1)F0 = vec3(0.95, 0.64, 0.54); // gold F0
	vec3 Ft = vec3(0.0f);
    vec3 Lt = vec3(0.0f);
    
	for(int i =0 ;i< numPLights;++i)
	{
        vec3 Lo = vec3(0.0f);
        vec3 L;
        float distance;
       
        if(existnormals)
        {
            L = normalize (TBNinverse * gpuLight[i].position.xyz- fragPos); // make it offline bro so it fast
            distance = length(TBNinverse * gpuLight[i].position.xyz- fragPos);
        }
        else{
	        L = normalize (gpuLight[i].position.xyz- fragPos);
            distance = length(gpuLight[i].position.xyz- fragPos);
        }
        float attenuation = 1.0 / (distance * distance); // attenuation based on inverse square law
   
		vec3 H = normalize (V+L);
        vec3 radiance = gpuLight[i].intensity * gpuLight[i].color.xyz * attenuation;
		// calculate Cook-Terrence specular BRDF 
		            // F
		            vec3 F = fresnelSchlick(max(dot(H,V),0.0f),F0);
                    // D
                    float NDF = DistributionGGX(N, H, roughness); 
                    // G
                    float G   = GeometrySmith(N, V, L, roughness);       
                    vec3 specular = (NDF * G * F) / (4*max(dot(N,V),0.0f)*max(dot(N,L),0.0f)+0.0001);
        Ft += F;
        vec3 ks = F;
        vec3 kd = vec3(1.0)- ks;
        kd *= (1.0-metallic);
        Lo = (kd * albedo / PI + specular) * radiance * (max(dot(N,L),0.0f));
        Lt += Lo;
	}
    
    vec3 color = Lt;
    vec3 emmisive = u_emmisive;
    if(existemmisive)
    {
        emmisive = vec3(texture(texture_emmisive,TexCoords));
    }
    float customFresnel = clamp(1-max(dot(N,V),0.0f),0,1);
    float intensityFresnel = 3.5;
    customFresnel = pow(customFresnel,intensityFresnel);
    color += emmisive + customFresnel;
    float exposure = 0.05;
    // HDR tonemap
    color = vec3(1.0) - exp(-color * exposure);
    //color = albedo;
    //color = color / (color + vec3(1.0));
    //vec3 x = max(vec3(0),color-0.004);
    //color = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
    //color = vec3(customFresnel)*1.0;
    // gamma correction 
    //color = srgbEncode(color);
    if(!existOpacity)
    {
        FragColor = vec4(vec3(color),(1.0f));
    }else{
        FragColor = vec4(vec3(color),texcolor.a);
        //FragColor = vec4(texcolor.a);
    }
    
}

// Function Definations
vec3 srgbEncode(vec3 color) {
    float r = color.r < 0.0031308 ? 12.92 * color.r : 1.055 * pow(color.r, 1.0/2.4) - 0.055;
    float g = color.g < 0.0031308 ? 12.92 * color.g : 1.055 * pow(color.g, 1.0/2.4) - 0.055;
    float b = color.b < 0.0031308 ? 12.92 * color.b : 1.055 * pow(color.b, 1.0/2.4) - 0.055;
    return vec3(r, g, b);
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