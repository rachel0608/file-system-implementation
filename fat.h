#ifndef FAT_H
#define FAT_H

#define BLOCK_SIZE 512

// Root directory is also a directory entry
typedef struct {
    char filename[8];
    char ext[3];
    uint16_t first_logical_cluster;
    uint32_t file_size;
    uint16_t type; // 0 for file, 1 for directory
} DirectoryEntry; 

typedef struct {
    DirectoryEntry entries[BLOCK_SIZE / sizeof(DirectoryEntry)];
} Directory; //holds 26 DirectoryEntrys

typedef struct {
    uint16_t block_number;
} FATEntry;

typedef struct {
    uint8_t bitmap[BLOCK_SIZE];
} BitmapBlock;

typedef struct {
    char buffer[BLOCK_SIZE];
} datablock; //holds a block of data

// find the directory entry for a file 
DirectoryEntry* find_file(char* filename);

// read file contents from clusters
void fat_read_file_contents(FATEntry *start_cluster, uint32_t file_size, uint8_t buffer);

// allocate a chain of clusters for a new file
void fat_allocate_cluster_chain(FATEntry* start_cluster, uint32_t file_size);

// update directory entry for a file   
void fat_update_directory_entry(const char* filename, FATEntry *start_cluster, uint32_t file_size);

// free clusters allocated to a file
void fat_free_cluster_chain(FATEntry *start_cluster);

#endif
