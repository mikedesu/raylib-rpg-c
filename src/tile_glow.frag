#version 330
in vec2 fragTexCoord;
out vec4 finalColor;
uniform sampler2D texture0;
uniform bool glow; // Toggle glow
void main() {
    vec4 color = texture2D(texture0, fragTexCoord);
    if (glow) {
        vec3 glowColor = vec3(1.0, 1.0, 0.0); // Yellow glow
        finalColor = vec4(mix(color.rgb, glowColor, 0.5), color.a);
    } else {
        finalColor = color;
    }
}
