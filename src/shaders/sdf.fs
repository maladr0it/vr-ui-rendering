#version 410 core

uniform sampler2D glyphAtlas;

in vec2 fragTexCoords;

out vec4 outColor;

void main() {
    vec3 lineColor = vec3(1.0, 0.0, 0.0);
    float lineWidth = 0.1;

    float dist = texture(glyphAtlas, fragTexCoords).r;

    outColor = vec4((1.0 - step(dist, (0.5 - lineWidth))) * lineColor, 1.0);

    // float opacity = // HERE

    // outColor = mix(vec4(0.0), vec4(lineColor, 0.0), )

    // float lineWidth = 0.4;

    // vec3 texCol = texture(glyphAtlas, fragTexCoords).rgb;

    // if (texCol.r < lineWidth) {
    //     outColor = vec4(bgColor, 1.0);
    // } else {
    //     outColor = vec4(lineColor, 0.0);
    // }
}