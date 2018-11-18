#ifndef UTILS_H
#define UTILS_H

char *read_file(char *filePath);
int mkdir_recursive(const char *dir);

char *strrev(char *str);
char *get_dirName(const char *ofFile);  // Returned data should be freed
char *get_fileName(const char *ofFile); // Returned data should be freed

#endif