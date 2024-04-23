#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("=== implementing ls ===\n");

    DIR *dir;                 // Pointer to the directory
    struct dirent *entry;    // Pointer to each directory entry

    char *path = argc > 1 ? argv[1] : "."; // Use current directory if no argument

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // Read and print each directory entry
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    // Close the directory
    closedir(dir);

    return 0;
}
