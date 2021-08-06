#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glad/glad.h>
#include "utils.h"
#include "shader.h"

static const int INFO_LOG_LEN = 512;

shader_t shader_create(char *vertexPath, char *fragmentPath)
{
    char vertexFullPath[512];
    char fragmentFullPath[512];
    utils_getFullPath(vertexPath, vertexFullPath);
    utils_getFullPath(fragmentPath, fragmentFullPath);

    int success;
    char infoLog[INFO_LOG_LEN];

    char *vertexShaderSource = utils_getFileContent(vertexFullPath);
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

    char *fragmentShaderSource = utils_getFileContent(fragmentFullPath);
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

void shader_setV2(shader_t program, char *name, v2_t v)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform2f(location, v.x, v.y);
}

void shader_setV3(shader_t program, char *name, v3_t v)
{
    int location = glGetUniformLocation(program.id, name);
    glUniform3f(location, v.x, v.y, v.z);
}

void shader_setMat4(shader_t program, char *name, mat4_t mat)
{
    int location = glGetUniformLocation(program.id, name);
    glUniformMatrix4fv(location, 1, GL_TRUE, (float *)(mat.m));
}

// TODO: rename args
void shader_getUniformString(char *name, int index, char *property, char *out)
{
    char *pOut = out;
    char *pName = name;
    while (*pName != '\0')
    {
        *pOut = *pName;
        ++pName;
        ++pOut;
    }

    char indexStr[16];
    utils_itoa(index, indexStr);
    char *pIndexStr = indexStr;

    *pOut = '[';
    ++pOut;
    while (*pIndexStr != '\0')
    {
        *pOut = *pIndexStr;
        ++pIndexStr;
        ++pOut;
    }
    *pOut = ']';
    ++pOut;

    char *pProperty = property;
    *pOut = '.';
    ++pOut;
    while (*pProperty != '\0')
    {
        *pOut = *pProperty;
        ++pProperty;
        ++pOut;
    }

    *pOut = '\0';
}