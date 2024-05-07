// testing relative_to_absolute, still need to fix

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH_LENGTH 260

// Function definition
char *relative_to_absolute(char *relative_path, char *current_directory, char *buffer) {
    // Use the provided buffer instead of allocating memory
    char *absolute_path = buffer;
    
    // Check if the relative path is already an absolute path
    if (relative_path[0] == '/') {
        strcpy(absolute_path, relative_path);
    } else {
        // Copy the current directory to the absolute path
        strcpy(absolute_path, current_directory);
        
        // Tokenize the relative path and handle each component
        char *token = strtok(relative_path, "/");
        while (token != NULL) {
            if (strcmp(token, ".") == 0) {
                // Current directory, do nothing
            } else if (strcmp(token, "..") == 0) {
                // Parent directory, remove the last directory from the absolute path
                char *last_slash = strrchr(absolute_path, '/');
                if (last_slash != NULL) {
                    *last_slash = '\0';
                }
            } else {
                // Append the directory or file name to the absolute path
                if (absolute_path[strlen(absolute_path) - 1] != '/') {
                    strcat(absolute_path, "/");
                }
                strcat(absolute_path, token);
            }
            token = strtok(NULL, "/");
        }
    }
    
    return absolute_path;
}

void test_absolute_path() {
    char current_directory[MAX_PATH_LENGTH];
    char relative_path[MAX_PATH_LENGTH];
    char absolute_path[MAX_PATH_LENGTH];

    strcpy(current_directory, "/home/user");
    strcpy(relative_path, "/etc/config.txt");
    printf("Test case 1: Absolute path\n");
    printf("Current directory: %s\n", current_directory);
    printf("Relative path: %s\n", relative_path);
    relative_to_absolute(relative_path, current_directory, absolute_path);
    printf("Absolute path: %s\n", absolute_path);
    printf("\n");
}

void test_relative_path_current_directory() {
    char current_directory[MAX_PATH_LENGTH];
    char relative_path[MAX_PATH_LENGTH];
    char absolute_path[MAX_PATH_LENGTH];

    strcpy(current_directory, "/home/user");
    strcpy(relative_path, "./documents/file.txt");
    printf("Test case 2: Relative path with current directory\n");
    printf("Current directory: %s\n", current_directory);
    printf("Relative path: %s\n", relative_path);
    relative_to_absolute(relative_path, current_directory, absolute_path);
    printf("Absolute path: %s\n", absolute_path);
    printf("\n");
}

void test_relative_path_parent_directory() {
    char current_directory[MAX_PATH_LENGTH];
    char relative_path[MAX_PATH_LENGTH];
    char absolute_path[MAX_PATH_LENGTH];

    strcpy(current_directory, "/home/user/documents");
    strcpy(relative_path, "../files/archive.zip");
    printf("Test case 3: Relative path with parent directory\n");
    printf("Current directory: %s\n", current_directory);
    printf("Relative path: %s\n", relative_path);
    relative_to_absolute(relative_path, current_directory, absolute_path);
    printf("Absolute path: %s\n", absolute_path);
    printf("\n");
}

void test_relative_path_multiple_levels() {
    char current_directory[MAX_PATH_LENGTH];
    char relative_path[MAX_PATH_LENGTH];
    char absolute_path[MAX_PATH_LENGTH];

    strcpy(current_directory, "/home/user");
    strcpy(relative_path, "documents/projects/source/main.c");
    printf("Test case 4: Relative path with multiple levels\n");
    printf("Current directory: %s\n", current_directory);
    printf("Relative path: %s\n", relative_path);
    relative_to_absolute(relative_path, current_directory, absolute_path);
    printf("Absolute path: %s\n", absolute_path);
    printf("\n");
}

int main() {
    test_absolute_path();
    test_relative_path_current_directory();
    test_relative_path_parent_directory();
    test_relative_path_multiple_levels();

    return 0;
}

// gcc -o test_translate_path test_translate_path.c
// ./test_translate_path