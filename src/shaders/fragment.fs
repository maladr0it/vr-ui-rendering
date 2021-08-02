#version 410 core

uniform sampler2D glyphAtlas;

in vec2 fragTexCoords;

out vec4 outColor;

void main() {
    vec3 texCol = texture(glyphAtlas, fragTexCoords).rgb;
    outColor = vec4(texCol, 1.0);
}