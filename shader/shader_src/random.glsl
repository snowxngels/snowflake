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
