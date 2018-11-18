#include "MetaFile.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct MetaFile *mf_create(const char *fileFullPath)
{
    struct MetaFile *toReturn = malloc(sizeof(struct MetaFile));

    char buf[1024];
    strcpy(buf, fileFullPath);

    toReturn->filePath = malloc(strlen(fileFullPath) + 1);
    strcpy(toReturn->filePath, buf);

    toReturn->fileFolderPath = get_dirName(buf);

    toReturn->fileName = get_fileName(buf);

    strcpy(buf, toReturn->fileName);
    strrev(buf);
    char *cursor = strchr(buf, '.');
    *(--cursor) = 'o';
    toReturn->objName = malloc(strlen(buf) + 1);
    strcpy(toReturn->objName, cursor);
    strrev(toReturn->objName);

    toReturn->dependencies = malloc(0);
    toReturn->nb_dependencies = 0;
    toReturn->needsCompilation = 0;

    return toReturn;
}

void mf_destroy(struct MetaFile *mf)
{
    free(mf->filePath);
    free(mf->fileFolderPath);
    free(mf->fileName);
    free(mf->objName);

    for (int i = 0; i < mf->nb_dependencies; i++)
        free(mf->dependencies[i]);
    free(mf->dependencies);

    free(mf);
}

int mf_hasDependency(struct MetaFile *mf, const char *file)
{
    for (int i = 0; i < mf->nb_dependencies; i++)
    {
        if (strcmp(mf->dependencies[i], file) == 0)
            return 1;
    }
    return 0;
}

void mf_addDependency(struct MetaFile *mf, const char *file)
{
    if (!mf_hasDependency(mf, file))
    {
        mf->dependencies = realloc(mf->dependencies, (++mf->nb_dependencies) * sizeof(char *));

        char *toAdd = malloc(strlen(file) + 1);
        strcpy(toAdd, file);
        mf->dependencies[mf->nb_dependencies - 1] = toAdd;
    }
}

void mf_addDependencyArray(struct MetaFile *mf, char **array, int sizeOfArray)
{
    for (int i = 0; i < sizeOfArray; i++)
        mf_addDependency(mf, array[i]);
}

void mf_print(struct MetaFile *mf)
{
    printf("%s:\n", mf->filePath);
    for (int i = 0; i < mf->nb_dependencies; i++)
        printf("\t%s\n", mf->dependencies[i]);
}