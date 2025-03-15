#version 330 core

out vec4 FragColor; 

in vec2 TexCoord; 
in vec3 FragPos;  
//in vec3 Normal;   
in vec3 Tangent;  
in mat3 TBN;

uniform sampler2D albedoTexture;         
uniform sampler2D normalTexture;         
uniform sampler2D metallicTexture;       
uniform sampler2D roughnessTexture;      
uniform sampler2D aoTexture;             

// Light properties
uniform vec3 light_source;                   
uniform vec3 cameraPos;                    
uniform vec3 light_color;                 

void main()
{

    vec3 albedo = texture(albedoTexture, TexCoord).rgb;
    vec3 normal = texture(normalTexture, TexCoord).rgb * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
    float metallic = texture(metallicTexture, TexCoord).r;
    float roughness = texture(roughnessTexture, TexCoord).r;
    float ao = texture(aoTexture, TexCoord).r;

    normal = normalize(normal);

    vec3 viewDir = -normalize(cameraPos - FragPos);

    vec3 lightDir = -normalize(light_source - FragPos);
    
    vec3 halfwayDir = -normalize(lightDir + viewDir);
//        vec3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse component
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = albedo * NdotL * ao; // Modulate by ambient occlusion

    // Specular component
    float NdotH = max(dot(normal, halfwayDir), 0.0);
    float VdotH = max(dot(viewDir, halfwayDir), 0.0);
    float D = pow(NdotH, 2.0); 
    float F = pow(1.0 - VdotH, 5.0); 
    float G = min(1.0, min(2.0 * NdotH * NdotL / VdotH, 2.0 * NdotH * VdotH / NdotL)); // Geometry term

    float specular = (D * F * G) / (4.0 * NdotL * VdotH + 0.001);

    vec3 finalColor = diffuse + specular * metallic;

    FragColor = vec4(finalColor, 1.0);

}