#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "filesystem.h"
#include "fat.h"

// filesystem.c

bool check_access(char* input) {
  int is_r = strcmp(input, 'r');
  int is_w = strcmp(input, 'w');
  int is_a = strcmp(input, 'a');

  if ((is_r == 0) || (is_w == 0) || (is_a == 0)) {
    return true;
  }

  return false;
}

FileHandle* f_open(char* filename, char* access) {
  if (check_access(access) == false) {
    printf("ERROR: Requested acces is not valid.\n");
    return NULL;
  }

  FileHandle* fileH;

  // Checking if the file exists
  DirectoryEntry* dir = f_opendir(filename); // Fix it to be path (we talked before how finding path will be a separate function(?)) !!!!!
  
  if (dir != NULL) {
    // Make FileHandle for this specific file info. // Ask the others(?) !!!!!
  } else if ((strcmp(access, 'w') == 0) || (strcmp(access, 'a') == 0)) {
    // Make new FileHandle with basic FileHandle info. // Ask the others(?) !!!!!
  } else { 
    printf("ERROR: Could not find specified file.\n");
    return NULL;
  }

  return fileH;
}

int f_read(FileHandle* file, void* buffer, size_t bytes) {
  int pos = file->position;

  if (pos < 0) {
    printf("ERROR: File position is not valid.\n");
    return 0; // Because Unix man said so: (Unix man says return short item count or zero)
  }

  int new_pos = pos + (int)(bytes);

  // Read bytes from the current position of file (FileHandle file’s position value)

  // fat_read_file_contents(FatEntry* start_cluster, uint32_t file_size, uint8_t buffer);
  return bytes;
}

DirectoryEntry* f_opendir(char* directory) { // Returns pointer to open directory
  // Find abs path of directory name using abs path function (?)
  DirectoryEntry* dirEntry;

  if (dirEntry == NULL) {
    printf("ERROR: Could not find specified directory.\n");
    return NULL;
  }

  // Open directory to read using directory (?) fix based on abs path  !!!!!
  // Just create directory entry  !!!!!

  return dirEntry;
}

DirectoryEntry* f_readdir(char* directory) { // Returns pointer to next directory
  // Read next directory entry
  DirectoryEntry* dirEntry;
  return dirEntry;
}
