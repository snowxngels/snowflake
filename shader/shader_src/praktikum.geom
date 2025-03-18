#version 330 core

layout(triangles) in; // Input primitive type
layout(triangle_strip, max_vertices = 6) out; // Output primitive type and max vertices

// Input from vertex shader
in vec3 TangentLightPos[];
in vec3 TangentViewPos[];
in vec3 TangentFragPos[];
in vec2 TexCoord[];
in vec3 FragPos[];
in mat4 modelViewMatrix[]; 

// Output to fragment shader
out vec3 oTangentLightPos;
out vec3 oTangentViewPos;
out vec3 oTangentFragPos;
out vec2 oTexCoord;
out vec3 oFragPos; 

void main() {
    // Set the output variables for the first vertex

for(int i = 0; i < 3; i++) {


    mat4 imodelViewMatrix = modelViewMatrix[0];
    mat4 invmodelViewMatrix = inverse(imodelViewMatrix);

    vec3 upVector = normalize(vec3(invmodelViewMatrix[0][1], invmodelViewMatrix[1][1], invmodelViewMatrix[2][1]));
    

    gl_Position = gl_in[i].gl_Position;
    gl_Position.y += 0.1;
    gl_Position += vec4(upVector * 1.0, 0.0);
    
    oTangentLightPos = TangentLightPos[i];
    oTangentViewPos = TangentViewPos[i];
    oTangentFragPos = TangentFragPos[i];
    oTexCoord = TexCoord[i];
    oFragPos = FragPos[i];

    EmitVertex(); // Emit the vertex


    gl_Position = gl_in[i].gl_Position;
    gl_Position.y += -0.1;
    
    oTangentLightPos = TangentLightPos[i];
    oTangentViewPos = TangentViewPos[i];
    oTangentFragPos = TangentFragPos[i];
    oTexCoord = TexCoord[i];
    oFragPos = FragPos[i];

    EmitVertex(); // Emit the vertex


    gl_Position = gl_in[i].gl_Position;
    gl_Position.x += 0.1;
    
    oTangentLightPos = TangentLightPos[i];
    oTangentViewPos = TangentViewPos[i];
    oTangentFragPos = TangentFragPos[i];
    oTexCoord = TexCoord[i];
    oFragPos = FragPos[i];

    EmitVertex(); // Emit the vertex

}
    EndPrimitive(); // End the primitive
}
