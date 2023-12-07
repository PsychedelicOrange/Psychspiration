#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec2 TexCoords2;
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
    //structs
struct GPULight {
    vec4 position;
    vec4 color;
    int enabled;
    float intensity;
    float range;
    float padding;
};

//uniforms
    //material values
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_emmisive;
    //textures
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughmetal;
uniform sampler2D texture_emmisive;
uniform sampler2D texture_lightmap;
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
    vec4 texlightmap = texture(texture_lightmap,TexCoords2);
    vec4 texcolor;
    vec3 albedo = u_albedo;
    float metallic = u_metallic;
    float roughness = u_roughness;
    if(existdiffuse)
    {
        texcolor = texture(texture_diffuse,TexCoords).rgba;
        if(texcolor.a < 0.1)
            discard;
        //albedo = pow(texcolor.rgb,vec3(2.2));
        albedo = texcolor.rgb;
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

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0,albedo,metallic);
	 
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
        vec3 radiance = ( gpuLight[i].intensity * gpuLight[i].color.xyz * attenuation * 0.001f );
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
        Lt += Lo;
	}

    vec3 emmisive = vec3(u_emmisive);
    if(existemmisive)
    {
        emmisive = vec3(texture(texture_emmisive,TexCoords));
    }
// lightmap
    
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 diffuse = texlightmap.rgb * albedo;
    vec3 ambient = (kD * diffuse);

    Lt += emmisive + ambient;//texlightmap.rgb;
    vec4 finalColor;
    if(!existOpacity)
    {
        finalColor = vec4(vec3(Lt),(1.0f));
    }else{
        finalColor = vec4(vec3(Lt),texcolor.a);
    }
    //finalColor = vec4(N,1.0);
    //float gamma = 1/0.9;
    //FragColor.rgb = pow(finalColor.rgb, vec3(1.0/gamma));
    FragColor = finalColor;
}

// Function Definations

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
