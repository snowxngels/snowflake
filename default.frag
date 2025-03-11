#version 330 core
out vec4 FragColor;

in vec3 NormalVec;
in vec2 TexCoord;
in vec3 FragPos;

//settings
uniform float disable_tex_shading;
uniform int affected_by_light;
uniform vec3 ambient_light_base;//base color (why did i name this so retarded)

//data
uniform sampler2D texture1;
uniform vec4 face_color;
uniform float face_brightness;
uniform vec3 cameraPos;

uniform vec3 light_color;
uniform float light_strength;
uniform int num_lights;
uniform vec3 light_source;

uniform float specular_strength;

void main()
{
    if(affected_by_light == 1) {

    	//texture shading
    	if(disable_tex_shading == 0.0f) {

	  // get direction from light onto fragment we render
	  vec3 norm = normalize(NormalVec);
	  vec3 lightDir = normalize(light_source - FragPos);

	  // get view direction vector and reflection direction vector
	  vec3 viewDir = normalize(cameraPos - FragPos);
	  vec3 reflectDir = reflect(-lightDir, norm);  // negate lightdir to make vec point towards surface (reflect needs that)

	  //calc specular component
	  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	  vec3 specular = specular_strength * spec * light_color;  

	  // calc diffuse (more aligned light dir and normal of frag -> more light)
	  float diff = max(dot(norm, lightDir), 0.0);
	  diff = clamp(diff, 0.0f, 0.4f);
	  vec3 diffuse = diff * light_color * light_strength;


	  vec3 clampedFinalColorRGB = clamp(texture(texture1, TexCoord).rgb, 0.0f, 0.5f);

	  FragColor = vec4(clampedFinalColorRGB * (ambient_light_base + diffuse + specular), 1.0f);

	}
	// face shading
	else {

	  // get direction from light onto fragment we render
	  vec3 norm = normalize(NormalVec);
	  vec3 lightDir = normalize(light_source - FragPos);

	  // calc diffuse (more aligned light dir and normal of frag -> more light)
	  float diff = max(dot(norm, lightDir), 0.0);
	  vec3 diffuse = diff * light_color;

	  vec3 clampedFinalColorRGB = clamp(face_color.rgb, 0.0f, 0.5f);

	  FragColor = vec4(clampedFinalColorRGB * (ambient_light_base + diffuse + light_strength), 1.0f);
	
	}

    }
    // unaffected by lighting
    else {
	if(disable_tex_shading == 0.0f) { FragColor = texture(texture1, TexCoord); }
	else { FragColor = face_color; }
    }
}