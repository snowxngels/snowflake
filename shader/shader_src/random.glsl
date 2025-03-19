#version 330 core
in vec2 fragCoord;
out vec4 color;

uniform float stripeWidth; 

void main() {
    // Calculate the stripe pattern based on the x coordinate
    float stripes = mod(floor(fragCoord.x / stripeWidth), 2.0);

    // Set color based on the stripe pattern
    if (stripes == 0.0) {
        color = vec4(1.0, 0.0, 0.0, 1.0); // Red
    } else {
        color = vec4(0.0, 0.0, 1.0, 1.0); // Blue
    }
}

----

layout(location = 0) in vec3 position;
layout(location = 1) in int counter; // Counter as an integer

out int fragCounter; // Pass the counter to the fragment shader

void main() {
    gl_Position = vec4(position, 1.0);
    fragCounter = counter; // Pass the counter value
}

- 

#version 330 core
in int fragCounter; // Receive the counter from the vertex shader
out vec4 color;

void main() {
    // Set color based on the counter value
    if (fragCounter == 0) {
        color = vec4(1.0, 0.0, 0.0, 1.0); // Red for counter 0
    } else {
        color = vec4(0.0, 0.0, 1.0, 1.0); // Blue for counter 1
    }
}


==========================0
#version 330 core
layout(location = 0) in vec3 position;

out vec3 fragPosition; // Pass the position to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragPosition = vec3(model * vec4(position, 1.0)); // Transform position to world space
    gl_Position = projection * view * vec4(fragPosition, 1.0);
}

-

#version 330 core
in vec3 fragPosition; // Position from vertex shader
out vec4 color;

uniform vec4 meshColor; // Base color of the mesh
uniform float outlineThickness; // Thickness of the outline

void main() {

    // Assuming a simple square billboard for demonstration
    float distanceToEdge = min(min(fragPosition.x, 1.0 - fragPosition.x), min(fragPosition.y, 1.0 - fragPosition.y));

    // Darken the color based on the distance to the edge
    float edgeFactor = smoothstep(0.0, outlineThickness, distanceToEdge); // Create a smooth transition
    vec4 shadedColor = meshColor * (1.0 - edgeFactor * 0.5); // Darken by 50% at edges

    // Set the final color
    color = shadedColor;
}
