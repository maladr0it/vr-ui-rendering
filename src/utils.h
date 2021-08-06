#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void *utils_malloc(size_t size);

char *utils_getFileContent(char *path);

void utils_getFullPath(char *relPath, char *buffer);

int utils_getToken(char *str, char delim, char *token, char **tokenEnd);

float utils_clampf(float val, float lower, float upper);

void utils_reverse(char *str, int len);

void utils_itoa(int n, char *str);

#endif