#version 330
in vec2 fragTexCoord;
out vec4 finalColor;
uniform sampler2D texture0;
uniform float time; // For pulsing effect

void main() {
    vec4 color = texture2D(texture0, fragTexCoord);
    vec3 glowColor = vec3(1.0, 1.0, 0.5); // Soft yellow glow
    float glowIntensity = 0.3 + 0.2 * sin(time * 2.0); // Pulse between 0.1 and 0.5
    vec3 glowed = mix(color.rgb, glowColor, glowIntensity);
    finalColor = vec4(glowed, color.a);
}
