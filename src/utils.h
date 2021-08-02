#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void *utils_malloc(size_t size);

char *utils_getFileContent(char *path);

char *utils_getFullPath(char *relPath, char *buffer);

int utils_getToken(char *str, char delim, char *token, char **tokenEnd);

float utils_clampf(float val, float lower, float upper);

#endif