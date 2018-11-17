#include "scan_dir.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

char *dynamic_strcat(char *original, const char *other) // Free previous memory and return heap allocated string
{
    size_t lenOri = strlen(original);
    size_t lenOth = strlen(other);

    char *toReturn = malloc(lenOri + lenOth + 1);
    strcpy(toReturn, original);
    strcat(toReturn, other);
    free(original);
    return toReturn;
}

char *scan_dir(const char *dirPath)
{
    char *toReturn = malloc(10);
    toReturn[0] = '\0';

    char *workingDir = malloc(strlen(dirPath) + 1);
    strcpy(workingDir, dirPath);

    DIR *dp = NULL;

    if ((dp = opendir(workingDir)) == NULL)
    {
        printf("ERROR\n");
        free(toReturn);
        free(workingDir);
        exit(EXIT_FAILURE);
        // ERROR
    }
    else
    {
        // Remove end / if present in directory path
        if (workingDir[strlen(workingDir) - 1] == '/')
            workingDir[strlen(workingDir) - 1] = '\0';

        for (struct dirent *de; (de = readdir(dp)) != NULL;)
        {
            if (de->d_type == DT_DIR)
            {
                // Exclude . and .. folders
                if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                    continue;

                char *newWorkingDir = malloc(strlen(workingDir) + strlen(de->d_name) + 2);
                sprintf(newWorkingDir, "%s/%s", workingDir, de->d_name);

                char *dataFromSubfolder = scan_dir(newWorkingDir);
                free(newWorkingDir);

                toReturn = dynamic_strcat(toReturn, dataFromSubfolder);
                free(dataFromSubfolder);
            }

            if (de->d_type == DT_REG)
            {
                //printf("%s/%s\n", workingDir, de->d_name);
                char *data = malloc(strlen(workingDir) + strlen(de->d_name) + 3);
                sprintf(data, "%s/%s\n", workingDir, de->d_name);
                toReturn = dynamic_strcat(toReturn, data);
                free(data);
            }
        }
        closedir(dp);
    }
    return toReturn;
}