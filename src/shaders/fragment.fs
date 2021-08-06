#version 410 core

uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;

uniform sampler2D specular1;
uniform sampler2D specular2;

in vec2 fragTexCoords;

out vec4 outColor;

void main() {
    vec3 texCol = texture(diffuse2, fragTexCoords).rgb;
    outColor = vec4(texCol, 1.0);
}