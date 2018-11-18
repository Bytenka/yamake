#ifndef COMPILE_INFO_H
#define COMPILE_INFO_H

struct CompileInfo
{
    char *compiler;
    char *outputDir;
    char **includeDirs;
    int nb_includeDirs;
};

struct CompileInfo *ci_create();
void ci_destroy(struct CompileInfo *ci);

void ci_setCompiler(struct CompileInfo *ci, const char *compiler);
void ci_setOutputDir(struct CompileInfo *ci, const char *outputDir);
void ci_addIncludeDir(struct CompileInfo *ci, const char *includeDir);

char *ci_generateCompileCommand(struct CompileInfo *ci, const char *forFile);
char *ci_generateLinkCommand(struct CompileInfo *ci);

#endif