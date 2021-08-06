#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoords;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main() {
  fragPos = vec3(model * vec4(vertPos, 1.0));
  fragTexCoords = vertTexCoords;

  // inversing a matrix is expensive, and only needs to be calculated once per model
  // ideally do it on the cpu and pass it as a uniform
  fragNormal = mat3(transpose(inverse(model))) * vertNormal;

  gl_Position = projection * view * model * vec4(vertPos, 1.0);
}

