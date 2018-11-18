#include "CompileInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CompileInfo *ci_create()
{
    struct CompileInfo *toReturn = malloc(sizeof(struct CompileInfo));

    toReturn->compiler = malloc(0);
    toReturn->outputDir = malloc(0);
    toReturn->includeDirs = malloc(0);
    toReturn->nb_includeDirs = 0;

    return toReturn;
}

void ci_destroy(struct CompileInfo *ci)
{
    free(ci->compiler);
    free(ci->outputDir);

    for (int i = 0; i < ci->nb_includeDirs; i++)
        free(ci->includeDirs[i]);
    free(ci->includeDirs);

    free(ci);
}

void ci_setCompiler(struct CompileInfo *ci, const char *compiler)
{
    ci->compiler = realloc(ci->compiler, strlen(compiler) + 1);
    strcpy(ci->compiler, compiler);
}

void ci_setOutputDir(struct CompileInfo *ci, const char *outputDir)
{
    ci->outputDir = realloc(ci->outputDir, strlen(outputDir) + 1);
    strcpy(ci->outputDir, outputDir);
}

void ci_addIncludeDir(struct CompileInfo *ci, const char *includeDir)
{
}

char *ci_generateCompileCommand(struct CompileInfo *ci, const char *forFile)
{
    char buf[1024];
    sprintf(buf, "%s -c -o %s", ci->compiler, ci->outputDir);
}

char *ci_generateLinkCommand(struct CompileInfo *ci)
{
}