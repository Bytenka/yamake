#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *read_file(char *filePath)
{
    FILE *fp = fopen(filePath, "r");
    if (!fp)
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        fseek(fp, 0L, SEEK_END);
        long fileSize = ftell(fp);
        rewind(fp);

        char *buffer = malloc(fileSize + 1);
        for (int c, i = 0; (c = getc(fp)) != EOF; i++)
        {
            buffer[i] = c;
        }
        buffer[fileSize] = '\0';

        fclose(fp);
        return buffer;
    }
}

// Template from https://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix
int mkdir_recursive(const char *dir)
{
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/') // Removes end slash if present
        tmp[len - 1] = 0;

    int error;
    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            if (*(p - 1) == '.') // Guards from attempting to create . and .. folders
                continue;

            *p = 0;

            error = mkdir(tmp, 0777);
            if (error != 0)
                return error;

            *p = '/';
        }
    }
    error = mkdir(tmp, 0777);
    return error;
}

/*******************************************************************/
/*
**  STRREV.C - reverse a string in place
**
**  public domain by Bob Stout
*/

char *strrev(char *str)
{
    char *p1, *p2;

    if (!str || !*str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}
/*******************************************************************/

char *get_dirName(const char *ofFile) // Strips last part of file path. Doesn't check if file is in fact a folder (so maybe could be improved @TODO)
{
    char buf[1024];
    strcpy(buf, ofFile);
    strrev(buf);

    int i = 0;
    for (; buf[i] != '/'; i++)
        ;
    i++;
    char *toReturn;

    if (buf[i] == '\0')
    {
        toReturn = malloc(2);
        sprintf(toReturn, "/");
        return toReturn;
    }

    toReturn = malloc(strlen(buf + i) + 1);
    strcpy(toReturn, buf + i);
    strrev(toReturn);

    return toReturn;
}

char *get_fileName(const char *ofFile) // Only keep the last field of a path
{
    char buf[1024];
    strcpy(buf, ofFile);
    strrev(buf);

    char *cursor = strchr(buf, '/');
    char *toReturn = NULL;

    if (cursor == buf)
    {
        toReturn = malloc(1);
        *toReturn = '\0';
        return toReturn;
    }

    *cursor = '\0';

    toReturn = malloc(strlen(buf) + 1);
    strcpy(toReturn, buf);
    strrev(toReturn);

    return toReturn;
}