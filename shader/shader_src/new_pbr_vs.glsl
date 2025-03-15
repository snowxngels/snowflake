#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aBinormal;
layout (location = 4) in vec3 aTangent;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec2 TexCoord;
out vec3 FragPos; 


uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

uniform vec3 light_source;
uniform vec3 cameraPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));   
    TexCoord = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);

    //vec3 B = cross(N, T);
    vec3 B = normalize(aBinormal);

    mat3 TBN = transpose(mat3(T, B, N));
    
    TangentLightPos = TBN * light_source;
    TangentViewPos  = TBN * cameraPos;
    TangentFragPos  = TBN * FragPos;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}