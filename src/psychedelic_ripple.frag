#version 330
in vec2 fragTexCoord;
out vec4 finalColor;
uniform sampler2D texture0;
uniform float time;      // Animation clock
uniform bool active;     // True if plate is pressed
uniform vec2 tilePos;    // Tile's (x, y) in dungeon coords

void main() {
    vec4 color = texture2D(texture0, fragTexCoord);
    if (active) {
        // Ripple distortion
        vec2 center = vec2(0.5, 0.5); // Tile center in texture coords
        vec2 offset = fragTexCoord - center;
        float dist = length(offset);
        float ripple = sin(dist * 20.0 - time * 5.0) * 0.05; // Wave amplitude
        vec2 warpedCoord = fragTexCoord + offset * ripple;

        // Psychedelic color shift
        vec3 rainbow = vec3(
            sin(time + tilePos.x * 0.1) * 0.5 + 0.5,
            sin(time + tilePos.y * 0.1 + 2.0) * 0.5 + 0.5,
            sin(time + tilePos.x * 0.1 + 4.0) * 0.5 + 0.5
        );
        color = texture2D(texture0, warpedCoord);
        finalColor = vec4(mix(color.rgb, rainbow, 0.7), color.a);
    } else {
        finalColor = color;
    }
}
