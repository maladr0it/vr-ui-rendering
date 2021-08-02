#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include "utils.h"
#include "shader.h"

static const int INFO_LOG_LEN = 512;

shader_t shader_create(char *vertexPath, char *fragmentPath)
{
    int success;
    char infoLog[INFO_LOG_LEN];

    char *vertexShaderSource = utils_getFileContent(vertexPath);
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, (const char *const *)&vertexShaderSource, NULL);
    glCompileShader(vertexShaderId);
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glad_glGetShaderInfoLog(vertexShaderId, INFO_LOG_LEN, NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    free(vertexShaderSource);

    char *fragmentShaderSource = utils_getFileContent(fragmentPath);
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, (const char *const *)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glad_glGetShaderInfoLog(fragmentShaderId, INFO_LOG_LEN, NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    free(fragmentShaderSource);

    shader_t program;
    program.id = glCreateProgram();
    glAttachShader(program.id, vertexShaderId);
    glAttachShader(program.id, fragmentShaderId);
    glLinkProgram(program.id);
    glad_glGetProgramiv(program.id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program.id, INFO_LOG_LEN, NULL, infoLog);
        printf("%s", infoLog);
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return program;
}

void shader_use(shader_t program)
{
    glUseProgram(program.id);
}

void shader_setBool(shader_t program, char *name, bool value)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform1i(location, (int)value);
}

void shader_setInt(shader_t program, char *name, int value)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform1i(location, value);
}

void shader_setFloat(shader_t program, char *name, float value)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform1f(location, value);
}

void shader_setMat4(shader_t program, char *name, mat4_t mat)
{
    int location = glGetUniformLocation(program.id, name);
    glUniformMatrix4fv(location, 1, GL_TRUE, (float *)(mat.m));
}

void shader_setV3(shader_t program, char *name, v3_t v)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform3f(location, v.x, v.y, v.z);
}