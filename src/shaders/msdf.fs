#version 410 core

uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;

uniform sampler2D specular1;
uniform sampler2D specular2;

in vec2 fragTexCoords;

out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 lineColor = vec3(0.0, 0.0, 0.0);
    float lineWidth = 0.2;

    vec3 msd = texture(diffuse1, fragTexCoords).rgb;
    float sd = 0.5 - median(msd.r, msd.g, msd.b);
    float dist = sd - lineWidth;

    float delta = fwidth(dist);
    float alpha = smoothstep(delta, -delta, dist);
    outColor = vec4(lineColor, alpha);
}