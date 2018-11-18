#ifndef METAFILE_H
#define METAFILE_H

struct MetaFile
{
    char *filePath;       // Full path to file
    char *fileFolderPath; // Full path to containing folder
    char *fileName;       // File name
    char *objName;        // file.o

    char **dependencies;
    int nb_dependencies;

    int needsCompilation;
};

struct MetaFile *mf_create(const char *fileFullPath);
void mf_destroy(struct MetaFile *mf);

int mf_hasDependency(struct MetaFile *mf, const char *file);
void mf_addDependency(struct MetaFile *mf, const char *file); // Discard if dependency already exists
void mf_addDependencyArray(struct MetaFile *mf, char **array, int sizeOfArray);
void mf_print(struct MetaFile *mf);

#endif