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

// Seek offset constants
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define MAX_DIRS 100
#define EMPTY 65535 // first_logical_cluster value for empty directory
#define END_OF_FILE 0 // FAT entry value for end of file
#define BLOCK_SIZE 512
#define FREEBLOCK 65535

// Root directory is also a directory entry
typedef struct {
    char filename[8];
    char ext[3];
    uint16_t first_logical_cluster;
    uint16_t type; // 0 for file, 1 for directory
    uint32_t file_size;
} DirectoryEntry; 

typedef struct {
    DirectoryEntry entries[BLOCK_SIZE / sizeof(DirectoryEntry)];
} Directory; //holds 26 DirectoryEntrys if 1MB disk

typedef struct {
    uint16_t block_number;
} FATEntry;

typedef struct {
    uint8_t bitmap[BLOCK_SIZE];
} BitmapBlock;

typedef struct {
    char buffer[BLOCK_SIZE];
} datablock; // holds a block of data

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
int f_remove(char* path);
//int f_stat(FileHandle file, struct stat *buffer);
int f_mkdir(char* path);
int f_rmdir(char* path);

// read file contents from clusters
void fat_read_file_contents(FATEntry *start_cluster, uint32_t file_size, uint8_t *buffer);

// allocate a chain of clusters for a new file
void fat_allocate_cluster_chain(FATEntry* start_cluster, uint16_t start_cluster_idx, uint32_t file_size);

void fat_update_directory_entry(const char* filename, FATEntry *start_cluster, uint32_t file_size);

#endif
