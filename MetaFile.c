#include "MetaFile.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct MetaFile mf_create(const char *codeFile)
{
    struct MetaFile toReturn = {NULL, NULL, 0};
    toReturn.codeFileName = malloc(strlen(codeFile) + 1);
    strcpy(toReturn.codeFileName, codeFile);
    return toReturn;
}

void mf_destroy(struct MetaFile *mf)
{
    free(mf->codeFileName);

    for (int i = 0; i < mf->nb_dependencies; i++)
        free(mf->dependencies[i]);
    free(mf->dependencies);
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
    if (mf->dependencies == NULL || !mf_hasDependency(mf, file))
    {
        if (mf->dependencies == NULL)
        {
            mf->dependencies = malloc(sizeof(char *));
            ++mf->nb_dependencies;
        }
        else
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
    printf("%s:\n", mf->codeFileName);
    for (int i = 0; i < mf->nb_dependencies; i++)
        printf("\t%s\n", mf->dependencies[i]);
}