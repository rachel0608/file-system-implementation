#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "filesystem.h" //structs: BootSector, FileHandle (OFTEntry), User, Group
//#include "fat.h" //structs: DirectoryEntry, FATEntry

#define DISK_SIZE_MB_DEFAULT 1
#define EXPECTED_ARGS 2
#define EXECUTABLE argv[0]
#define FILENAME argv[1]
#define FLAG argv[2]
#define FLAG_PRESENT 4
#define FLAG_PARAM argv[3]
#define MAX_NAME 8
#define BLOCKSIZE 512
#define SUPERSIZE 1
#define FATSIZE 8
#define ROOTSIZE 1

typedef struct {
    uint16_t block_size;        // block size will be standard 512 bytes
    uint8_t file_size_mb;       // file size in MB (default is 1MB)
    uint32_t file_size_blocks;  // file size in blocks (1MB = 2048 blocks)
    uint32_t freeblock_offset;  // offset of first freeblock location
    uint8_t FAT_offset;         // offset of first FAT block location
    uint8_t FREEMAP_offset      // offset of first Free-Bitmap location
    uint8_t ROOTDIR_offset;     // offset of first ROOTDIR block location
    uint8_t DATA_offset;        // offset of first DATABLOCK location
} superblock;

superblock initialize_superblock(disk_image, disk_size_mb);

int main(int argc, char *argv[]) {

    // Parse command-line arguments
    if (argc < EXPECTED_ARGS) {
        fprintf(stderr, "Usage: %s <filename> -s <num MB requested>\n", EXECUTABLE);
        return EXIT_FAILURE;
    }

    int disk_size_mb = DISK_SIZE_MB_DEFAULT;
    char* filename = FILENAME;

    if (argc > EXPECTED_ARGS && strcmp(FLAG, "-s") == 0) {
        disk_size_mb = atoi(FLAG_PARAM);
    }

    // Open disk
    FILE *disk_image = fopen(filename, "wb"); //if exists will open existing, otw will create new

    //if disk existed, modify structure
    //if disk was created, write to size disk_size_mb

    //question: how do you deal with a disk that already exists? It will not have consistent format

    // Initialize disk image with Superblock, FAT, RootDirEntry
    superblock sb = initialize_superblock(disk_image, disk_size_mb);
    test_sb(sb);
    test_readsb(disk_image);

    fclose(disk_image);
    printf("Disk image formatted successfully: %s\n", filename);
    return EXIT_SUCCESS;
}

superblock initialize_superblock(disk_image, disk_size_mb) {

    superblock sb;
    sb.block_size = BLOCKSIZE;
    sb.file_size_mb = disk_size_mb;
    sb.file_size_blocks = (disk_size_mb * 1024) / (BLOCKSIZE / 1024);
    sb.freeblock_offset = -1;
    sb.FAT_offset = SUPERSIZE;
    sb.FREEMAP_offset = SUPERSIZE + FATSIZE;
    sb.ROOTDIR_offset = SUPERSIZE + FATSIZE + FREEMAPSIZE;
    sb.DATA_offset = SUPERSIZE + FATSIZE + FREEMAPSIZE + ROOTSIZE
        
    //seek to 0
    write_to_disk(disk_image, sb, sizeof(superblock));
    return sb;
}

void test_sb(superblock sb) {
    printf("Block Size = %d\n", sb.block_size);
    printf("File Size in MB = %d\n", sb.file_size_mb);
    printf("File Size in Blocks = %d\n", sb.file_size_blocks);
    printf("Freeblock Offset = %d\n", sb.freeblock_offset);
    printf("FAT Offset = %d\n", sb.FAT_offset);
    printf("ROOTDIR Offset = %d\n", sb.ROOTDIR_offset);
    printf("DATA Offset = %d\n", sb.DATA_offset);


}

void test_readsb(FILE* disk_image) {

}

