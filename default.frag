#version 330 core
out vec4 FragColor;

in vec3 NormalVec;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture1;

uniform vec4 face_color;

uniform float render_type;

uniform vec3 ambient_light_base;

uniform float face_brightness;

uniform int affected_by_light;

uniform int num_lights;

uniform vec3 light_color;

uniform vec3 light_source;

uniform float light_strength;

void main()
{
    if(affected_by_light == 1) {
    
	vec3 norm = normalize(NormalVec);
	vec3 lightDir = normalize(light_source - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;

	float clampedRenderType = clamp(render_type, 0.0, 1.0);

	vec3 finalColor = mix(texture(texture1, TexCoord).rgb, face_color.rgb, clampedRenderType);

	vec3 clampedFinalColor = clamp(finalColor, 0.0, 0.5);

	FragColor = vec4(clampedFinalColor * (ambient_light_base + diffuse * light_strength), 1.0f);

    } else {
        FragColor = mix(texture(texture1, TexCoord), face_color, render_type);
    }
}