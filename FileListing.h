#ifndef FILELISTING_H
#define FILELISTING_H

struct FileListing
{
    char **sourceFiles;
    char **codeFiles;
    char **headerFiles;

    int nb_sourceFiles;
    int nb_codeFiles;
    int nb_headerFiles;
};

struct FileListing *fl_create();
void fl_destroy(struct FileListing *fl);

void fl_addSource(struct FileListing *fl, const char *file);
void fl_addCode(struct FileListing *fl, const char *file);
void fl_addHeader(struct FileListing *fl, const char *file);

int fl_sourceContainsFile(struct FileListing *fl, const char *file);
int fl_codeContainsFile(struct FileListing *fl, const char *file);
int fl_headerContainsFile(struct FileListing *fl, const char *file);

void fl_print(struct FileListing *fl);

#endif