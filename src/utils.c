#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

void *utils_malloc(size_t size)
{
    void *result = malloc(size);
    if (result == NULL)
    {
        printf("failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    return result;
}

// TODO: unused
int utils_getToken(char *str, char delim, char *token, char **tokenEnd)
{
    int tokenLength = 0;
    char *p = str;

    while (*p == delim && *p != '\0' && *p != '\n')
    {
        ++p;
    }

    while (*p != delim && *p != '\0' && *p != '\n')
    {
        token[tokenLength] = *p;
        ++tokenLength;
        ++p;
    }

    token[tokenLength] = '\0';
    *tokenEnd = p;
    return tokenLength;
}

char *utils_getFileContent(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("failed to open file: %s", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    char *content = utils_malloc((len + 1) * sizeof(char));

    fseek(file, 0, SEEK_SET);
    fread(content, sizeof(char), len, file);
    fclose(file);

    content[len] = '\0';
    return content;
}

void utils_getFullPath(char *relPath, char *buffer)
{
    uint32_t programPathLen = 512;

    // TODO: this only works for mac
    if (_NSGetExecutablePath(buffer, &programPathLen) != 0)
    {
        printf("");
        exit(EXIT_FAILURE);
    }

    // programPathLen doesn't seem to be updated by _NSGetExecutablePath, so we don't know its length and must go from the start
    char *pBuffer = buffer;
    char *pLastSlash = buffer;
    char *pRelPath = relPath;
    while (*pBuffer != '\0')
    {
        if (*pBuffer == '/')
        {
            pLastSlash = pBuffer;
        }
        ++pBuffer;
    }

    pBuffer = pLastSlash + 1;

    while (*pRelPath != '\0')
    {
        *pBuffer = *pRelPath;
        ++pBuffer;
        ++pRelPath;
    }

    *pBuffer = '\0';
}

float utils_clampf(float val, float lower, float upper)
{
    if (val < lower)
    {
        return lower;
    }
    else if (val > upper)
    {
        return upper;
    }
    else
    {
        return val;
    }
}

void utils_reverse(char *str, int len)
{
    char temp;
    for (int i = 0; i < len / 2; i++)
    {
        temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void utils_itoa(int n, char *str)
{
    int i = 0;
    int is_negative = n < 0;

    do
    {
        str[i] = (is_negative ? -1 : 1) * (n % 10) + '0';
        n /= 10;
        i++;
    } while (n != 0);

    if (is_negative)
    {
        str[i] = '-';
        i++;
    }

    utils_reverse(str, i);
    str[i] = '\0';
}