precision mediump float;

varying mediump vec2 fragTexCoord;
uniform sampler2D texture0;
uniform mediump float time; // For pulsing effect

void main() {
    mediump vec4 color = texture2D(texture0, fragTexCoord);
    // red glow
    vec3 glowColor = vec3(1.0, 0.0, 0.0); 
    // Pulse between 0.1 and 0.5
    float glowIntensity = 0.3 + 0.2 * sin(time * 2.0); 
    vec3 glowed = mix(color.rgb, glowColor, glowIntensity);
    gl_FragColor = vec4(glowed, color.a);
}
