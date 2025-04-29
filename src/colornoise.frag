#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform float time;

//
// Simple hash-based pseudo-random function (returns float in [0,1])
//
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898,78.233)) + time * 3.14159) * 43758.5453);
}

void main() {
    // Generate random color components for this pixel
    float r = rand(fragTexCoord * 75.0 + 1.0);
    float g = rand(fragTexCoord * 90.0 + 2.0);
    float b = rand(fragTexCoord * 65.0 + 3.0);
    // Animate pattern over time
    r = rand(fragTexCoord * 75.0 + time * 2.3 + 1.0);
    g = rand(fragTexCoord * 90.0 + time * 1.7 + 2.0);
    b = rand(fragTexCoord * 65.0 + time * 3.1 + 3.0);

    finalColor = vec4(r, g, b, 1.0);
}
