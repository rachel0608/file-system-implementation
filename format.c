/**
 * format.c
 * Authors: czhang, gchoe, jrieger, rnguyen
 * Date: 5/11/2024
 * 
 * Description:
 * Implementation and initialization of a disk image with a custom file system
 * structure, including a superblock, FAT table, bitmap for block allocation,
 * and a root directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
#define FREEMAP_SLOTS 4096
#define BYTES_PER_ENTRY 2
#define CEIL_ROUNDING 1
#define KB_IN_MB 1024.0
#define FREEBLOCK 65535
#define END_OF_FILE 0
#define ROOTDIR 0
#define BITS_PER_BLOCK (BLOCKSIZE * 8)
#define DIR_SIZE_CAP 26
#define DIR 1


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
    DirectoryEntry entries[DIR_SIZE_CAP];
} Directory; //holds 16 DirectoryEntrys

typedef struct {
    char buffer[BLOCKSIZE]; //either Directory or File
} datablock; //holds a block of data

superblock initialize_superblock(FILE *disk_image, int disk_size_mb); //calculate DS sizes and init & write superblock
void test_sb(superblock sb);
void test_readsb(FILE *disk_image);
void initialize_fat(FILE *disk_image, superblock sb);
void test_readfat(FILE *disk_image, superblock sb);
void test_fat_end_of_chain(FILE *disk_image, superblock sb);
void test_cluster_allocation(FILE *disk_image, superblock sb);
void initialize_bitmap(FILE *disk_image, superblock sb);
void test_bitmap(FILE *disk_image, superblock sb);
void initialize_rootdir(FILE *disk_image, superblock sb);

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

    // open to write- initialize disk image with Superblock, FAT, Bitmap, RootDirEntry
    FILE *disk_image = fopen(filename, "wb"); //write to size disk_size_mb
    
    superblock sb = initialize_superblock(disk_image, disk_size_mb);
    initialize_fat(disk_image, sb);
    initialize_bitmap(disk_image, sb);
    initialize_rootdir(disk_image, sb);

    fclose(disk_image);

    // open to read
    disk_image = fopen(filename, "rb"); //write to size disk_size_mb

    fclose(disk_image);
    
    printf("Disk image formatted successfully: %s\n", filename);
    return EXIT_SUCCESS;
}

//calculate DS sizes and init & write superblock
superblock initialize_superblock(FILE *disk_image, int disk_size_mb) {
    superblock sb;
    sb.block_size = BLOCKSIZE;
    sb.file_size_mb = disk_size_mb;
    sb.file_size_blocks = (disk_size_mb * KB_IN_MB) / (BLOCKSIZE / KB_IN_MB);
    int fatsize = (sb.file_size_blocks * BYTES_PER_ENTRY + BLOCKSIZE - CEIL_ROUNDING)/BLOCKSIZE; //FAT16 entries are 2 bytes each * num blocks, ceil
    int freemap_size = (sb.file_size_blocks + FREEMAP_SLOTS - CEIL_ROUNDING)/FREEMAP_SLOTS; //1 bitmap block per 4096 freeblocks, ceil divide
    int rootsize = 1;
    sb.FAT_offset = SUPERSIZE;
    sb.FREEMAP_offset = SUPERSIZE + fatsize;
    sb.ROOTDIR_offset = SUPERSIZE + fatsize + freemap_size;
    sb.DATA_offset = SUPERSIZE + fatsize + freemap_size + rootsize;
        
    //long int current_position = ftell(disk_image);
    //printf("FP: %ld\n", current_position);

    fseek(disk_image, 0, SEEK_SET);
    fwrite(&sb, sizeof(superblock), 1, disk_image);
    
    return sb;

}

void initialize_fat(FILE *disk_image, superblock sb) {
    //each block of FAT has 256 entries
    int num_blocks = sb.file_size_blocks;

    // Allocate memory & initialize all FAT entries as free
    FATEntry *fat_table = (FATEntry *)malloc(num_blocks * sizeof(FATEntry));

    for (int i = 0; i < num_blocks; i++) {
        fat_table[i].block_number = FREEBLOCK;
    }
    fat_table[ROOTDIR].block_number = END_OF_FILE; //first block is reserved for the root dir

    // Write FAT to disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fwrite(fat_table, (num_blocks * sizeof(FATEntry)), 1, disk_image);

    free(fat_table);
}

void initialize_bitmap(FILE *disk_image, superblock sb) {
    BitmapBlock bitmap_block;

    // Initialize the bitmap block with all bits set to 1 (indicating free) except first reserved block
    memset(bitmap_block.bitmap, 0xFF, BLOCKSIZE);
    bitmap_block.bitmap[ROOTDIR / 8] &= ~(1 << (ROOTDIR % 8)); //set root dir block (first) to used

    // Write the bitmap block to the disk image
    fseek(disk_image, (BLOCKSIZE * sb.FREEMAP_offset), SEEK_SET); // Move to the position after the boot sector
    fwrite(&bitmap_block, sizeof(BitmapBlock), 1, disk_image);

    /* How to set first 10 blocks as used:
    for (int i = 0; i < 10; i++) {
        bitmap_block.bitmap[i / 8] &= ~(1 << (i % 8)); // Set corresponding bit to 0 (used)
    }
    */
}

void initialize_rootdir(FILE *disk_image, superblock sb) {
    //the root dir block will be a normal dirEntry that points to the first datablock, reserved for rootdir contents
    DirectoryEntry root_dir_entry;
    strcpy(root_dir_entry.filename, "/"); //filename of root directory entry should be "/"
    root_dir_entry.first_logical_cluster = ROOTDIR; //points to first datablock-> holds directory for root level
    root_dir_entry.file_size = ROOTDIR;
    root_dir_entry.type = DIR;
    fseek(disk_image, (BLOCKSIZE * sb.ROOTDIR_offset), SEEK_SET);
    fwrite(&root_dir_entry, sizeof(DirectoryEntry), 1, disk_image); //Directory Entry is size: 32 bytes

    //DirectoryEntrys are 32 bytes each -> can fit 16 entrys in each directory block
    Directory root_dir;
    DirectoryEntry curr_dir_entry;
    strcpy(curr_dir_entry.filename, "."); // Filename of current directory entry should be "."
    curr_dir_entry.first_logical_cluster = ROOTDIR;      // Points to first data block
    curr_dir_entry.file_size = ROOTDIR;        // Dirs set to size 0
    curr_dir_entry.type = DIR;
    root_dir.entries[ROOTDIR] = curr_dir_entry; // Assuming the root directory is at index 0

    fseek(disk_image, (BLOCKSIZE * (sb.DATA_offset + ROOTDIR)), SEEK_SET); // Move to first block in data section
    fwrite(&root_dir, sizeof(Directory), 1, disk_image); //Directory will take up the first reserved block
}

void test_sb(superblock sb) {
    printf("Block Size = %d\n", sb.block_size);
    printf("File Size in MB = %d\n", sb.file_size_mb);
    printf("File Size in Blocks = %d\n", sb.file_size_blocks);
    printf("FAT Offset = %d\n", sb.FAT_offset);
    printf("FREEMAP Offset = %d\n", sb.FREEMAP_offset);
    printf("ROOTDIR Offset = %d\n", sb.ROOTDIR_offset);
    printf("DATA Offset = %d\n\n", sb.DATA_offset);
}

void test_readsb(FILE *disk_image) {
    superblock sb;
    fseek(disk_image, 0, SEEK_SET);
    fread(&sb, sizeof(superblock), 1, disk_image);
    test_sb(sb);
}

void test_readfat(FILE *disk_image, superblock sb) {
    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, sb.file_size_blocks * sizeof(FATEntry), 1, disk_image);

    for (int i = 0; i < (int) sb.file_size_blocks; i++) {
        printf("fat cell %d: %d\n", i, fat_entries[i].block_number);
    }
}

void test_fat_end_of_chain(FILE *disk_image, superblock sb) {
    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, sb.file_size_blocks * sizeof(FATEntry), 1, disk_image);

    // Simulate file allocation (chain of clusters)
    int start_cluster = 2000;
    int current_cluster = start_cluster;

    while (fat_entries[current_cluster].block_number != END_OF_FILE) {
        current_cluster = fat_entries[current_cluster].block_number;
        printf("Next cluster in chain: %d\n", current_cluster);
    }

    // read back and verify FAT entries for the file's clusters
    test_readfat(disk_image, sb);
    
    // Verify that last cluster in chain points to end of chain
    printf("Last cluster in chain: %d\n", current_cluster);
}

void test_cluster_allocation(FILE *disk_image, superblock sb) {
    // Assuming you have initialized FATEntry array and disk_image file pointer
    FATEntry fat_entries[sb.file_size_blocks];

    // Read FAT entries from disk
    fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    fread(fat_entries, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk_image);

    // Simulate file allocation (chain of clusters)
    int start_cluster = 2000;
    int current_cluster = start_cluster;

    // Allocate clusters for a new file (chain them in FAT)
    while (current_cluster < (int) sb.file_size_blocks) {
        if (fat_entries[current_cluster].block_number == FREEBLOCK) {
            fat_entries[current_cluster].block_number = END_OF_FILE;
            break;
        }
        current_cluster++;
    }

    // Write updated FAT entries back to disk
    //fseek(disk_image, (BLOCKSIZE * sb.FAT_offset), SEEK_SET);
    //fwrite(fat_entries, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk_image);

    // read back and verify FAT entries for the allocated clusters
    test_readfat(disk_image, sb);
}

//tests bitmap with first 10 blocks used
void test_bitmap(FILE *disk_image, superblock sb) {
    BitmapBlock bitmap_block;

    // Read the bitmap block from the disk image
    fseek(disk_image, BLOCKSIZE * sb.FREEMAP_offset, SEEK_SET); // Move to the position after the boot sector
    fread(&bitmap_block, sizeof(BitmapBlock), 1, disk_image);

    // Validate the bitmap contents
    int num_used_blocks = 0;
    for (int i = 0; i < (int) sb.file_size_blocks; i++) {
        int byte_index = i / 8;
        int bit_index = i % 8;
        int is_used = !(bitmap_block.bitmap[byte_index] & (1 << bit_index));

        if (is_used) {
            printf("Block %d is used\n", i);
            num_used_blocks++;
        }
    }

    if (num_used_blocks == 10) {
        printf("Bitmap initialization test passed: Expected 10 used blocks\n");
    } else {
        printf("Bitmap initialization test failed: Expected 10 used blocks, found %d\n", num_used_blocks);
    }
}
