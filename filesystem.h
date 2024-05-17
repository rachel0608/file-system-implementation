/**
 * filesystem.h
 * Authors: czhang, gchoe, jrieger, rnguyen
 * Date: 5/11/2024
 * 
 * Description:
 * A header for the filesystem.
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fat.h"

// Seek offset constants
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define MAX_DIRS 100
#define EMPTY 65535 // first_logical_cluster value for empty directory
#define END_OF_FILE 0 // FAT entry value for end of file

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
	char* abs_path;             // Absolute path
    int first_logical_cluster;  // First logical cluster
	int file_desc;              // File descriptor
	char* access;               // File access type
	int position;               // File position
    uint32_t file_size;
} FileHandle; // FileHandle loosely based on Unix systems’s FileHandle (may not need?)

extern superblock sb;
extern FATEntry *FAT = NULL;
extern BitmapBlock bitmap;
extern DirectoryEntry root_dir_entry;
extern Directory *root_dir = NULL;
extern datablock *data_section = NULL;

FileHandle* f_open(char* path, char* access); // access = w/w+, r/r+, a/a+ maybe add group later as param
int f_read(FileHandle *file, void* buffer, int bytes);
int f_close(FileHandle* file);
int f_seek(FileHandle* file, long offset, int whence);
void f_rewind(FileHandle* file);
DirectoryEntry* f_opendir(char* path);
Directory* f_readdir(DirectoryEntry* entry);
int f_closedir(DirectoryEntry* dir_entry);
void fs_mount(char *diskname);
void fs_unmount(char *diskname);

int f_write(FileHandle *file, void* buffer, size_t bytes);
int f_remove(const char* path);
//int f_stat(FileHandle file, struct stat *buffer);
int f_mkdir(char* path);
int f_rmdir(char* path);

#endif
