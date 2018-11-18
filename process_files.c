#include "utils.h"
#include "process_files.h"
#include "FileListing.h"
#include "MetaFile.h"
#include "CompileInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

char **get_dependencies(const char *ofFile_path, int *processedCount);
struct FileListing *build_fileListing(const char *filesList);
struct MetaFile **build_metaFiles(struct FileListing *fl, int *nb_metafiles);

void process_files(const char *filesList, const char *buildDirectory)
{
    //---------- Identifying files  ----------//
    struct FileListing *fl = build_fileListing(filesList);
    { // Scope of fl

        int nb_metafiles;
        struct MetaFile **mf = build_metaFiles(fl, &nb_metafiles);

        { // Scope of mf
            struct CompileInfo *ci = ci_create();

            {
                ci_setCompiler(ci, "gcc");
                ci_setOutputDir(ci, buildDirectory);

                compile_metaFiles(mf, nb_metafiles, ci);

            } // ci
            ci_destroy(ci);
        } // mf

        for (int i = 0; i < nb_metafiles; i++)
            mf_destroy(mf[i]);
        free(mf);
    } // fl
    fl_destroy(fl);
}

// Search the file for #include statements, and build the path to the corresponding file
// Returns heap allocated array of paths
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

// Takes string as input containing files to analyse, and builds a FileListing struct with
// only the usefull files.
// One line of filesList = one file. Each line should be '\n' terminated, last one included
struct FileListing *build_fileListing(const char *filesList)
{

    char *files = malloc(strlen(filesList) + 1); // Heap buffer, size can be huge
    strcpy(files, filesList);

    struct FileListing *fl = fl_create(); // Holds sources and headers references (absolute paths)

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
            fl_addCode(fl, cursor);
        }
        if (strcmp(buf, "pph") == 0 || strcmp(buf, "h") == 0)
        {
            addToSrc++;
            fl_addHeader(fl, cursor);
        }
        if (addToSrc)
            fl_addSource(fl, cursor);

        *endOfLine = '\n'; // Technically not needed
        // --------------- //
        cursor = endOfLine + 1;
    }
    free(files);

    return fl;
}

// Uses data from a FileListing to generate a MetaFile, holding usefull build information
struct MetaFile **build_metaFiles(struct FileListing *fl, int *nb_metafiles)
{
    *nb_metafiles = 0;
    struct MetaFile **metafiles = malloc(0);

    for (int i = 0; i < fl->nb_codeFiles; i++)
    {
        struct MetaFile *mf = mf_create(fl->codeFiles[i]);

        // Get dependencies of code file
        int deptsCount;
        char **depts = get_dependencies(mf->filePath, &deptsCount);
        mf_addDependencyArray(mf, depts, deptsCount);

        for (int j = 0; j < deptsCount; j++)
            free(depts[j]);
        free(depts);

        // Resolve dependencies of dependencies
        if (deptsCount > 0)
        {
            // This is recursive because files are added to the end of the array,
            // so everything that is added will be processed. No cycling deps either
            for (int j = 0; j < mf->nb_dependencies; j++)
            {
                depts = get_dependencies(mf->dependencies[j], &deptsCount);
                mf_addDependencyArray(mf, depts, deptsCount);

                for (int k = 0; k < deptsCount; k++)
                    free(depts[k]);
                free(depts);
            }
        }

        // Push new metafile to array
        metafiles = realloc(metafiles, (++(*nb_metafiles)) * sizeof(struct MetaFile *));
        metafiles[*nb_metafiles - 1] = mf;
    }

    return metafiles;
}

void compile_metaFiles(struct MetaFile **mf, int nb_metafiles, struct CompileInfo *ci)
{
    for (int f = 0; f < nb_metafiles; f++)
    {
        char objOfThisFile[1024];
        sprintf(objOfThisFile, "%s/%s", ci->outputDir, mf[f]->objName);
        if (access(objOfThisFile, F_OK) == -1)
        {
            // File does not exist, set and continue
            mf[f]->needsCompilation = 1;
            continue;
        }
        else
        {
            struct stat attribSrc;
            struct stat attribObj;
            stat(mf[f]->filePath, &attribSrc);
            stat(objOfThisFile, &attribObj);

            // Checking if code file has been modified
            if (difftime(attribObj.st_mtime, attribSrc.st_mtime) < 0)
            {
                mf[f]->needsCompilation = 1;
                continue;
            }

            // Checking if dependencies have been modified
            for (int d = 0; d < mf[f]->nb_dependencies; d++)
            {
                stat(mf[f]->dependencies[d], &attribSrc);
                if (difftime(attribObj.st_mtime, attribSrc.st_mtime) < 0)
                {
                    mf[f]->needsCompilation = 1;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < nb_metafiles; i++)
    {

        if (mf[i]->needsCompilation)
        {
            printf("Compiling: %s\n", mf[i]->filePath);
            char buf[1024];
            sprintf(buf, "gcc %s -c -o %s/%s -I/home/hugo/Dropbox/OpenGL/Libraries/Includes", mf[i]->filePath, ci->outputDir, mf[i]->objName);
            system(buf);
        }
    }
}
