#ifndef METAFILE_H
#define METAFILE_H

struct MetaFile
{
    char *codeFileName;
    char **dependencies;
    int nb_dependencies;
};

struct MetaFile mf_create(const char *codeFile);
void mf_destroy(struct MetaFile *mf);

int mf_hasDependency(struct MetaFile *mf, const char *file);
void mf_addDependency(struct MetaFile *mf, const char *file); // Discard if dependency already exists
void mf_addDependencyArray(struct MetaFile *mf, char **array, int sizeOfArray);
void mf_print(struct MetaFile *mf);

#endif