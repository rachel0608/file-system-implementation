#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fat.h"

typedef struct {
    uint8_t file_size_mb;       // file size in MB (default is 1MB)
    uint8_t FAT_offset;         // offset of first FAT block location
    uint8_t FREEMAP_offset;      // offset of first Free-Bitmap location
    uint8_t ROOTDIR_offset;     // offset of first ROOTDIR block location
    uint8_t DATA_offset;        // offset of first DATABLOCK location
    uint16_t block_size;        // block size will be standard 512 bytes
    uint32_t file_size_blocks;  // file size in blocks (1MB = 2048 blocks)
} superblock;

typedef struct {
	char* abs_path; // Absolute path
	int file_desc; // File descriptor
	char* access; // File access type
	int position; // File position
} FileHandle; // FileHandle loosely based on Unix systems’s FileHandle (may not need?)

FileHandle* f_open(char* path, char* access); // access = w/w+, r/r+, a/a+ maybe add group later as param
int f_read(FileHandle file, void* buffer, size_t bytes);
int f_write(FileHandle file, void* buffer, size_t bytes);
void f_close(FileHandle* file);
// int f_seek(FileHandle* file, long offset, int whence);
// void f_rewind(FileHandle file);
// int f_stat(FileHandle file, struct stat *buffer);
int f_remove(const char* path);
DirectoryEntry* f_opendir(char* path);
// DirectoryEntry* f_readdir(char* path);
int f_closedir(char* path);
int f_mkdir(char* path);
int f_rmdir(char* path);
void fs_mount(char *diskname);

// f_mount(); // Extra credit
// f_umount(); // Extra credit

#endif
