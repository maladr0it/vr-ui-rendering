#version 410 core

uniform sampler2D glyphAtlas;

in vec2 fragTexCoords;

out vec4 outColor;

void main() {
    vec3 lineColor = vec3(0, 1.0, 0);
    float lineWidth = 0.2;

    float sd = 0.5 - texture(glyphAtlas, fragTexCoords).r;
    float dist = sd - lineWidth;

    float delta = fwidth(dist);
    float alpha = smoothstep(delta, -delta, dist);
    
    outColor = vec4(lineColor, alpha);
}