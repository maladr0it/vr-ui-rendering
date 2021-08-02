#version 410 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertTexCoords;

out vec2 fragTexCoords;

void main()
{
    fragTexCoords = vertTexCoords;
    gl_Position = projection * view * model * vec4(vertPos, 1.0f);
}