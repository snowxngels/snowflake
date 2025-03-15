#version 330 core

out vec4 FragColor; 

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec2 TexCoord;
in vec3 FragPos; 

uniform sampler2D albedoTexture;         
uniform sampler2D normalTexture;         
uniform sampler2D metallicTexture;       
uniform sampler2D roughnessTexture;      
uniform sampler2D aoTexture;             

void main()
{

    vec3 normal = texture(normalTexture, TexCoord).rgb;

    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    vec3 color = texture(albedoTexture, TexCoord).rgb;

    vec3 ambient = 0.1 * color;

    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(viewDir, 1.0);  

}