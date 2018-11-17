#ifndef PROCESS_FILES_H
#define PROCESS_FILES_H

void process_files(const char *filesList);
char **get_dependencies(const char *ofFile, int *processedCount); // Returned array should be recursively freed
char *get_dirName(const char *ofFile);                            // Returned data should be freed

#endif