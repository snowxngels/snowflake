#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aBinormal;
layout (location = 4) in vec3 aTangent;

out vec2 TexCoord;
out vec3 FragPos; 
out vec3 Normal;  
out vec3 Tangent;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection;

void main()
{
//    FragPos = vec3(model * vec4(aPos, 1.0));
  //  Normal = mat3(transpose(inverse(model))) * aNormal;
    //Tangent = mat3(transpose(inverse(model))) * aTangent;

    TexCoord = aTexCoord * 15;
    FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 Binormal = normalize(vec3(model * vec4(aBinormal, 0.0)));
    Normal = normalize(vec3(model * vec4(aNormal, 0.0)));
    Tangent = normalize(vec3(model * vec4(aTangent, 0.0)));


    mat3 TBN = mat3(Tangent, Binormal, Normal);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}