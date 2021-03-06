#include "FileListing.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct FileListing *fl_create()
{
    struct FileListing *toReturn = malloc(sizeof(struct FileListing));

    toReturn->sourceFiles = malloc(0);
    toReturn->codeFiles = malloc(0);
    toReturn->headerFiles = malloc(0);

    toReturn->nb_sourceFiles = 0;
    toReturn->nb_headerFiles = 0;
    toReturn->nb_codeFiles = 0;

    return toReturn;
}

void fl_destroy(struct FileListing *fl)
{
    for (int i = 0; i < fl->nb_sourceFiles; i++)
    {
        free(fl->sourceFiles[i]);
    }
    free(fl->sourceFiles);

    for (int i = 0; i < fl->nb_codeFiles; i++)
    {
        free(fl->codeFiles[i]);
    }
    free(fl->codeFiles);

    for (int i = 0; i < fl->nb_headerFiles; i++)
    {
        free(fl->headerFiles[i]);
    }
    free(fl->headerFiles);

    free(fl);
}

void fl_addSource(struct FileListing *fl, const char *file)
{
    fl->sourceFiles = realloc(fl->sourceFiles, (++fl->nb_sourceFiles) * sizeof(char *));

    char *toAdd = malloc(strlen(file) + 1);
    strcpy(toAdd, file);
    fl->sourceFiles[fl->nb_sourceFiles - 1] = toAdd;
}

void fl_addCode(struct FileListing *fl, const char *file)
{
    fl->codeFiles = realloc(fl->codeFiles, (++fl->nb_codeFiles) * sizeof(char *));

    char *toAdd = malloc(strlen(file) + 1);
    strcpy(toAdd, file);
    fl->codeFiles[fl->nb_codeFiles - 1] = toAdd;
}

void fl_addHeader(struct FileListing *fl, const char *file)
{
    fl->headerFiles = realloc(fl->headerFiles, (++fl->nb_headerFiles) * sizeof(char *));

    char *toAdd = malloc(strlen(file) + 1);
    strcpy(toAdd, file);
    fl->headerFiles[fl->nb_headerFiles - 1] = toAdd;
}

int fl_sourceContainsFile(struct FileListing *fl, const char *file)
{
    for (int i = 0; i < fl->nb_sourceFiles; i++)
        if (strcmp(fl->sourceFiles[i], file) == 0)
            return 1;

    return 0;
}

int fl_codeContainsFile(struct FileListing *fl, const char *file)
{
    for (int i = 0; i < fl->nb_codeFiles; i++)
        if (strcmp(fl->codeFiles[i], file) == 0)
            return 1;

    return 0;
}

int fl_headerContainsFile(struct FileListing *fl, const char *file)
{
    for (int i = 0; i < fl->nb_headerFiles; i++)
        if (strcmp(fl->headerFiles[i], file) == 0)
            return 1;

    return 0;
}

void fl_print(struct FileListing *fl)
{
    for (int i = 0; i < fl->nb_sourceFiles; i++)
        printf("%s\n", fl->sourceFiles[i]);
    printf("\n");
    for (int i = 0; i < fl->nb_codeFiles; i++)
        printf("%s\n", fl->codeFiles[i]);
    printf("\n");
    for (int i = 0; i < fl->nb_headerFiles; i++)
        printf("%s\n", fl->headerFiles[i]);
    printf("\n");
}