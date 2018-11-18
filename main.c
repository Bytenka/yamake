#include "scan_dir.h"
#include "utils.h"
#include "process_files.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

void print_usage_and_exit()
{
    printf("usage:\n");
    printf("\t[-s /path/to/sources]\n");
    printf("\t[-b /output/objects/to]\n");
    exit(EXIT_FAILURE);
}

char *set_sources_dir(const char *to) // Return pointer to memory with absolute path. Memory should be freed
{
    char buf[1024];
    if (realpath(to, buf) == NULL)
    {
        printf("Could not resolv path %s\n", to);
        exit(EXIT_FAILURE);
    }

    char *sourcesDir = malloc(strlen(buf) + 1);
    strcpy(sourcesDir, buf);

    return sourcesDir;
}

char *set_build_dir(const char *to) // Return pointer to memory with absolute path. Memory should be freed
{
    char buf[1024];
    if (realpath(to, buf) == NULL)
    {
        // Checks if directory exists
        struct stat st = {0};
        if (stat(to, &st) == -1)
        {
            if (mkdir_recursive(to) != 0)
            {
                printf("Unable to create build directory %s\n", to);
                exit(EXIT_FAILURE);
            }

            realpath(to, buf); // Retest
        }
        else
        {
            printf("Unable to resolve build directory %s\n", to);
            exit(EXIT_FAILURE);
        }
    }

    char *buildDir = malloc(strlen(buf) + 1);
    strcpy(buildDir, buf);

    return buildDir;
}

int main(int argc, char *argv[])
{
    char *sourcesDir = set_sources_dir(".");
    char *buildDir = set_build_dir("./build");

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--sources-directory") == 0)
        {
            if (i + 1 >= argc)
            {
                print_usage_and_exit();
            }
            else
            {
                free(sourcesDir);
                sourcesDir = set_sources_dir(argv[++i]);
            }
        }
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--build-directory") == 0)
        {
            if (i + 1 >= argc)
            {
                print_usage_and_exit();
            }
            else
            {
                free(buildDir);
                buildDir = set_build_dir(argv[++i]);
            }
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--build-args") == 0)
        {
        }
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            print_usage_and_exit();
        }
    }

    printf("Scanning directory for files... (Dir=%s)\n", sourcesDir);

    char *data = scan_dir(sourcesDir);

    // Get discovered files count
    int count = 0;
    const char *cursor = data;
    while ((cursor = strstr(cursor, "\n")))
    {
        count++;
        cursor++;
    }
    printf("Found [%d] files in directory and sub.\n\n", count);
    //---------------------------//

    process_files(data, buildDir);
    free(data);

    free(sourcesDir);
    free(buildDir);

    return 0;
}

///*

//*/