#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
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

#define NR_POINT_LIGHTS 1

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TBNinverse;
uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform samplerCube depthMap;
uniform float far_plane;
uniform bool doshadows;
uniform bool donormals;
uniform bool existnormals;
// function prototypes
float ShadowCalculation(vec3 fragPos);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos, vec3 viewDir,bool doshadows,bool normals);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ambient_strength =0.7f;
void main()
{   vec3 normal;
    vec3 viewDir;
    vec3 fragPos;
    bool normals = donormals && existnormals;
    if (normals)
    {
    normal = texture(material.texture_normal1,TexCoords).rgb;
    normal = normalize(normal * 2.0 -1.0); 
    viewDir = normalize(TangentViewPos - TangentFragPos);
    fragPos = TangentFragPos;
    }
    else
    {
    normal = Normal;
    viewDir = normalize(viewPos - FragPos);
    fragPos = FragPos;

    }
    
    vec3 result = vec3(0.0f);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
     result += CalcPointLight(pointLights[i], normal,fragPos, viewDir,doshadows, normals);
    }    
    //phase 3: spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);   
   
 
    FragColor = vec4(result, 1.0);
    //gamma correction
    //float gamma = 2.2;
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,bool doshadows,bool normals)
{
    vec3 lightDir;
    float distance; 
    if(normals)
    {
    float distance = length(TBNinverse * light.position - fragPos);
    lightDir =  normalize(TBNinverse* light.position - fragPos);
    //return (Normal);
    }
    else{
    float distance = length(light.position - fragPos);
    lightDir =  normalize(light.position - fragPos);
    //return normal;
    }
     
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
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    ;
    // combine results
    vec3 ambient = ambient_strength * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.color * spec * vec3(texture(material.texture_specular1, TexCoords));
    //vec3 specular = light.color * spec * vec3(1.0);
    //diffuse = diff * vec3(1.0f);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = doshadows ? ShadowCalculation(FragPos) : 1.0;                      
    return (ambient + (shadow)*(diffuse + specular));
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