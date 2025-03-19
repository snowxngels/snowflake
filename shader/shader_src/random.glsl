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


