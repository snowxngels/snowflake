#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec4 face_color;
uniform float render_type;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), face_color, render_type);
}