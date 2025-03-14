#version 400

/////////////////////
// INPUT VARIABLES //
/////////////////////
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBinormal;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out vec2 texCoords;
out vec3 normal;
out vec3 tangent;
out vec3 binormal;
out vec3 viewDirection;


///////////////////////
// UNIFORM VARIABLES //
///////////////////////
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
    vec4 worldPosition;

    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    texCoords = aTexCoord;    
    normal = mat3(transpose(inverse(model))) * aNormal;
    tangent = mat3(transpose(inverse(model))) * aTangent;
    binormal = mat3(transpose(inverse(model))) * aBinormal;

    worldPosition = model * vec4(aPos, 1.0f);
    viewDirection = cameraPos.xyz - worldPosition.xyz;
    viewDirection = normalize(viewDirection);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}