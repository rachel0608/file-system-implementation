#define _POSIX_SOURCE 199309L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCKSIZE 512

// for shell command implementation, see folder: shell_commands

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
    uint16_t block_number;  // 16-bit value (2 bytes) representing block number (after DATA_offset)
} FATEntry;

typedef struct {
    uint8_t bitmap[BLOCKSIZE]; // Bitmap to track free data blocks
} BitmapBlock;

typedef struct {
    char filename[8];
    char ext[3];
    uint16_t first_logical_cluster;
    uint32_t file_size;
    uint16_t type; // 0 for file, 1 for directory
} DirectoryEntry; //19 bytes

typedef struct {
    DirectoryEntry entries[26];
} Directory; //holds 26 DirectoryEntrys

typedef struct {
    char buffer[BLOCKSIZE];
} datablock; //holds a block of data

void fs_mount(char *diskname);

superblock sb;
FATEntry *fat;
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
datablock *data_section;

int main() {


    fs_mount("DISK");

  
    //main loop
    
    return 0;
}


//to be called before the mainloop
void fs_mount(char *diskname) {

    FILE *disk = fopen(diskname, "rb");
    if (disk == NULL) {
        fprintf(stderr, "Error: Failed to open disk file '%s'\n", diskname);
        exit(EXIT_FAILURE);
    }
    printf("file opened\n");

    //extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func

    //read + define superblock
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(superblock), 1, disk);
    printf("-----------------------");
    printf("\nsb read and defined:\n");
    printf("Block Size = %d\n", sb.block_size);
    printf("File Size in MB = %d\n", sb.file_size_mb);
    printf("File Size in Blocks = %d\n", sb.file_size_blocks);
    printf("FAT Offset = %d\n", sb.FAT_offset);
    printf("FREEMAP Offset = %d\n", sb.FREEMAP_offset);
    printf("ROOTDIR Offset = %d\n", sb.ROOTDIR_offset);
    printf("DATA Offset = %d\n\n", sb.DATA_offset);

    int num_blocks = sb.file_size_blocks;
    fat = (FATEntry *)malloc(num_blocks * sizeof(FATEntry));

    //read + define FAT
    fseek(disk, BLOCKSIZE * sb.FAT_offset, SEEK_SET);
    fread(fat, (num_blocks * sizeof(FATEntry)), 1, disk);
    printf("---------------------------------------------------------");
    printf("\nfat read and defined: uncomment to print fat contents\n");
    for (int i = 0; i < num_blocks; i++) {
        //Uncomment to print fat contents
        printf("fat cell %d: %d\n", i, fat[i].block_number); 
    }

    //read + define bitmap
    fseek(disk, BLOCKSIZE * sb.FREEMAP_offset, SEEK_SET);
    fread(&bitmap, sizeof(BitmapBlock), 1, disk);
    printf("-----------------------------------------------------------------");
    printf("\nbitmap read and defined: check cecilia's bitmap code for util\n");

    //read + define rootdirentry
    fseek(disk, BLOCKSIZE * sb.ROOTDIR_offset, SEEK_SET);
    fread(&root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 19 bytes
    printf("-----------------------------------");
    printf("\nroot dir entry read and defined:\n");
    printf("filename: %s\n", root_dir_entry.filename);
    printf("ext: %s\n", root_dir_entry.ext);
    printf("first block: %d\n", root_dir_entry.first_logical_cluster);
    printf("file size: %d\n", root_dir_entry.file_size);
    printf("file type: %d\n", root_dir_entry.type);

    data_section = (datablock *)malloc(num_blocks * sizeof(data_section));

    //read + define datablock section
    fseek(disk, BLOCKSIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
    fread(data_section, BLOCKSIZE, sb.file_size_blocks, disk);
    printf("--------------------------------");
    printf("\ndata section read and defined:\n");
    Directory *root_dir = (Directory *) data_section[0].buffer;

    // Access the first block in the data section to verify the copy
    printf("Root dir's first entry filename: %s\n", root_dir->entries[0].filename);
    printf("Root dir's first entry file size: %d\n", root_dir->entries[0].file_size);
    printf("Root dir's first entry first block: %d\n", root_dir->entries[0].first_logical_cluster);
    printf("Root dir's first entry type: %d\n", root_dir->entries[0].type);

    fclose(disk);
}
