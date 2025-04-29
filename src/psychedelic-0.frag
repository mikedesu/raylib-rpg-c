#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main() {
    // Subtle "wavy" coordinate distortion
    float freq1 = 10.0;
    float freq2 = 5.0;
    float amp1 = 0.025;
    float amp2 = 0.025;

    // Compute offsets based on pixel position and time
    float waveX = amp1 * sin(fragTexCoord.y * freq1 + time * 1.3);
    float waveY = amp2 * cos(fragTexCoord.x * freq2 + time * 1.9);

    // Adjust sample coordinates
    vec2 uv = fragTexCoord + vec2(waveX, waveY);

    // Sample the original texture using warped coordinates
    vec4 color = texture(texture0, uv);

    // Optional: color shift for psychedelic effect (subtle)
    float shift = 0.06 * sin(time + fragTexCoord.x * 30.0);
    vec3 shifted = vec3(color.r + shift, color.g, color.b - shift);

    finalColor = vec4(shifted, color.a);
}
