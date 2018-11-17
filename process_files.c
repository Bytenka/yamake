#include "utils.h"
#include "process_files.h"
#include "FileListing.h"
#include "MetaFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_files(const char *filesList)
{
    char *files = malloc(strlen(filesList) + 1);
    strcpy(files, filesList);

    //---------- Setup file object ----------//

    struct FileListing fl = fl_create();

    char *cursor = files;

    while (*cursor != '\0')
    {
        char *endOfLine = strchr(cursor, '\n');

        // Temporary split //
        *endOfLine = '\0';

        char buf[1024];
        strcpy(buf, cursor);
        strrev(buf);

        char *dot = strchr(buf, '.');
        if (dot == NULL)
        {
            cursor = endOfLine + 1;
            continue;
        }
        *dot = '\0';

        int addToSrc = 0;
        if (strcmp(buf, "ppc") == 0 || strcmp(buf, "c") == 0)
        {
            addToSrc++;
            fl_addCode(&fl, cursor);
        }
        if (strcmp(buf, "pph") == 0 || strcmp(buf, "h") == 0)
        {
            addToSrc++;
            fl_addHeader(&fl, cursor);
        }
        if (addToSrc)
            fl_addSource(&fl, cursor);

        *endOfLine = '\n'; // Technically not needed
        // --------------- //
        cursor = endOfLine + 1;
    }

    free(files);

    //----------- Resolving dependencies ------------
    for (int i = 0; i < fl.nb_codeFiles; i++)
    {
        struct MetaFile mf = mf_create(fl.codeFiles[i]);

        // Get dependencies of code file
        int deptsCount;
        char **depts = get_dependencies(mf.codeFileName, &deptsCount);
        mf_addDependencyArray(&mf, depts, deptsCount);

        for (int j = 0; j < deptsCount; j++)
            free(depts[j]);
        free(depts);

        // Resolve dependencies of dependencies
        if (deptsCount > 0)
        {
            // This is recursive because files are added to the end of the array,
            // so everything that is added will be processed. No cycling deps either
            for (int j = 0; j < mf.nb_dependencies; j++)
            {
                depts = get_dependencies(mf.dependencies[j], &deptsCount);
                mf_addDependencyArray(&mf, depts, deptsCount);

                for (int k = 0; k < deptsCount; k++)
                    free(depts[k]);
                free(depts);
            }
        }
        mf_print(&mf);
        printf("\n");

        mf_destroy(&mf);
    }

    fl_destroy(&fl);
}

char **get_dependencies(const char *ofFile_path, int *processedCount)
{
    char **toReturn = NULL;
    *processedCount = 0;
    FILE *fp = fopen(ofFile_path, "r");

    if (!fp)
    {
        printf("Unable to open file %s\n", ofFile_path);
        exit(EXIT_FAILURE);
    }
    else
    {
        char line[2048];
        while (fgets(line, sizeof(line), fp) != NULL)
        {
            if (*line == '#')
            {
                char *cursor;
                if ((cursor = strstr(line, "include")) != NULL)
                {
                    cursor += 7; // Size of "include"

                    while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n') // Skip white spaces
                        cursor++;

                    if (*cursor == '"')
                    {
                        char fileName[256];

                        int i = 0;
                        for (cursor++; *(cursor + i) != '"'; i++)
                            fileName[i] = *(cursor + i);

                        fileName[i] = '\0';

                        char *dirName = get_dirName(ofFile_path);
                        char filePath[1024];
                        sprintf(filePath, "%s/%s", dirName, fileName);
                        free(dirName);

                        char *resolvedPath = malloc(strlen(filePath) + 1);
                        resolvedPath = realpath(filePath, resolvedPath);

                        if (toReturn == NULL)
                            toReturn = malloc(++(*processedCount) * sizeof(char *));
                        else
                            toReturn = realloc(toReturn, ++(*processedCount) * sizeof(char *));
                        toReturn[*processedCount - 1] = resolvedPath;

                        //printf("%s\n", fileName);
                    }
                }
            }
        }
        fclose(fp);
    }
    return toReturn;
}

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