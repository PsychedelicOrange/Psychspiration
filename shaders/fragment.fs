#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};
struct DirLight {
    vec3 direction;
    vec3 color;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;      
};
uniform samplerCube depthMap; 
#define NR_POINT_LIGHTS 1

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform DirLight dirLight;
uniform Material material;
uniform float far_plane;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 fragPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ambient_strength =0.7f;
float near =1.0f;
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far_plane) / (far_plane + near - z * (far_plane - near));	
}
float ShadowCalculation(vec3 fragPos, vec3 normal, vec3 pos)
{
    vec3 fragToLight = FragPos - pos;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, fragToLight).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = length(fragToLight);
    float bias = max(0.05 * (1.0-dot(normal,normalize(fragToLight))),0.005); 
    // check whether current frag pos is in shadow
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    //float shadow = closestDepth;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    //float shadow = currentDepth;
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    return shadow;
}
void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    //directional lights
    vec3 result = vec3(0.0);
    //result = CalcDirLight(dirLight, norm, viewDir,FragPos);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
    result += CalcPointLight(pointLights[0], norm, FragPos, viewDir);
    }   
   // phase 3: spot light
   // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);   
   
   
    //result = texture(shadowMap,TexCoords).rgb;
    FragColor = vec4(result, 1.0);
    //gamma correction
    float gamma = 1;
      
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float spec;
    // diffuse shading
    float diff = max(dot(normal, lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // specular shading
    //phong
    //spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    //blinn phong
    spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    if(diff == 0 )
    {
        spec = 0;
    }

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = ambient_strength * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.color * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = ShadowCalculation(fragPos,normal,light.position);                      
    //return (ambient +diffuse + specular);
    return (ambient +((1-shadow)* (diffuse + specular)));
    //return ((1.0 - shadow) * (vec3(1,1,1)));
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
vec3 ambient = ambient_strength * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.color * spec * vec3(texture(material.texture_specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 fragPos)
{
    //vec3 lightDir = normalize(light.direction - fragPos);
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = ambient_strength * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.color * spec * vec3(texture(material.texture_specular1, TexCoords));
    float shadow = ShadowCalculation(fragPos,normal,light.direction);                    
    //return (ambient +diffuse + specular);
    return (ambient +((1-shadow)* (diffuse + specular)));
    // return (shadow *vec3(1,1,1));
    //return (ambient + diffuse + specular);
}
