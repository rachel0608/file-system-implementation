/**
 * fake_disk_2.c
 * Authors: czhang, gchoe, jrieger, rnguyen
 * Date: 5/11/2024
 * 
 * Description:
 * Implementation of a fake disk.
 *
 * Layout: superblock (1) (0) | FAT (8) (1-8) | FREEMAP (1) (9) | ROOTDIR (1) (10) | DATA (11 reserved, start at 12)
 *
 * Layout of the fake_disk_2.c:
 * - `fake_disk_2.c` generates 'fake_disk_2.img' (3-level) with 4 folders and 1 file
 *    root
 *    |__ folder1
 *    |   |__ folder_a
 *    |   |__ folder_b (only in fake_disk_4_folders.img)
 *    |
 *    |__ folder2 (empty)
 *    |
 *    |__ Hello.txt (contains data "Hello")
 * 
 * Superblock:
 * File size: 1 MB
 * FAT offset: 1
 * Free map offset: 9
 * Root directory offset: 10
 * Data offset: 12
 * Block size: 512
 * File size in blocks: 2048
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "filesystem.h"

#define BLOCK_SIZE 512
#define FAT_SIZE 8 // in blocks
#define ROOT_DIR_SIZE 1
#define EMPTY 65535 // first_logical_cluster value for empty directory

void writeSuperblock(FILE *disk) {
    superblock sb;
    sb.file_size_mb = 1;
    sb.FAT_offset = 1;
    sb.FREEMAP_offset = 9;
    sb.ROOTDIR_offset = 10;
    sb.DATA_offset = 11;
    sb.block_size = BLOCK_SIZE;
    sb.file_size_blocks = 1 * (1024 * 1024) / BLOCK_SIZE;
    
    fwrite(&sb, sizeof(superblock), 1, disk);
}

void readSuperblock(FILE *disk) {
    superblock sb;
    fread(&sb, sizeof(superblock), 1, disk);
    printf("Superblock Info:\n");
    printf("File size: %d MB\n", sb.file_size_mb);
    printf("FAT offset: %d\n", sb.FAT_offset);
    printf("Free map offset: %d\n", sb.FREEMAP_offset);
    printf("Root directory offset: %d\n", sb.ROOTDIR_offset);
    printf("Data offset: %d\n", sb.DATA_offset);
    printf("Block size: %d\n", sb.block_size);
    printf("File size in blocks: %d\n", sb.file_size_blocks);
    printf("\n");
}   

void writeFAT(FILE *disk) {
    int fat_entry_count = FAT_SIZE * BLOCK_SIZE;
    FATEntry FAT[fat_entry_count];
    memset(FAT, 0, sizeof(FAT)); // init all FAT to 0
    fseek(disk, BLOCK_SIZE * 1, SEEK_SET); // FAT starts at block 1
    fwrite(&FAT, sizeof(FATEntry), fat_entry_count, disk);
}

void readFAT(FILE *disk) {
    int fat_entry_count = FAT_SIZE * BLOCK_SIZE;
    FATEntry FAT[fat_entry_count];
    fseek(disk, BLOCK_SIZE * 1, SEEK_SET); // FAT starts at block 1
    fread(&FAT, sizeof(FATEntry), fat_entry_count, disk);

    printf("FAT Info:\n");
    for (int i = 0; i < fat_entry_count; i++) {
        printf("%d ", FAT[i].block_number);
    }
    printf("\n");
}

void writeBitmap(FILE *disk) {
    BitmapBlock bitmap;
    memset(bitmap.bitmap, 1, BLOCK_SIZE);

    for (int i = 0; i < 3; i++) {
        bitmap.bitmap[i] = 0;
    }

    fseek(disk, BLOCK_SIZE * 9, SEEK_SET); // Bitmap starts at block 9
    fwrite(&bitmap, sizeof(BitmapBlock), 1, disk);
}

void readBitmap(FILE *disk) {
    BitmapBlock bitmap;
    fseek(disk, BLOCK_SIZE * 9, SEEK_SET); // Bitmap starts at block 9
    fread(&bitmap, sizeof(BitmapBlock), 1, disk);

    printf("Bitmap Info:\n");
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%d", (bitmap.bitmap[i]));
    }
    printf("\n");
}

void writeRootDir(FILE *disk) {
    DirectoryEntry rootDir;
    strcpy(rootDir.filename, "/");
    strcpy(rootDir.ext, "");
    rootDir.first_logical_cluster = 0;
    rootDir.file_size = 0;
    rootDir.type = 1; // directory

    fseek(disk, BLOCK_SIZE * 10, SEEK_SET); // Root directory starts at block 10
    fwrite(&rootDir, sizeof(DirectoryEntry), 1, disk);
}

void readRootDir(FILE *disk) {
    DirectoryEntry rootDir;
    fseek(disk, BLOCK_SIZE * 10, SEEK_SET); // Root directory starts at block 10
    fread(&rootDir, sizeof(DirectoryEntry), 1, disk);
    printf("Root directory: %s\n", rootDir.filename);
}

// Write subdirectories and files in root directory
void writeSubDir(FILE *disk) {
    DirectoryEntry subDir[3]; // 2 folders, 1 file
    
    memcpy(subDir[0].filename, "folder1", 8);
    memcpy(subDir[0].ext, "", 3);
    subDir[0].first_logical_cluster = 1;
    subDir[0].file_size = 0;
    subDir[0].type = 1; // directory

    memcpy(subDir[1].filename, "folder2", 8);
    memcpy(subDir[1].ext, "", 3);
    subDir[1].first_logical_cluster = EMPTY;
    subDir[1].file_size = 0;
    subDir[1].type = 1; // directory

    memcpy(subDir[2].filename, "file1", 8);
    memcpy(subDir[2].ext, "txt", 3);
    subDir[2].first_logical_cluster = 2;
    subDir[2].file_size = 5;
    subDir[2].type = 0; // file

    fseek(disk, BLOCK_SIZE * 11, SEEK_SET); // Subdirectories start at block 11
    fwrite(&subDir, sizeof(DirectoryEntry), 3, disk);
}

void writeData(FILE *disk) {
    // write data for folder1 in logical cluster 1
    DirectoryEntry subDir[2];
    memcpy(subDir[0].filename, "folder_a", 8); // folder1 stores folder_a
    memcpy(subDir[0].ext, "", 3);
    subDir[0].first_logical_cluster = EMPTY;
    subDir[0].file_size = 0;
    subDir[0].type = 1; // directory

    memcpy(subDir[1].filename, "folder_b", 8); // folder1 stores folder_a
    memcpy(subDir[1].ext, "", 3);
    subDir[1].first_logical_cluster = EMPTY;
    subDir[1].file_size = 0;
    subDir[1].type = 1; // directory
    fseek(disk, BLOCK_SIZE * (11+1), SEEK_SET); 
    fwrite(&subDir, sizeof(DirectoryEntry), 2, disk);

    // write data for file1.txt in logical cluster 2
    char data[BLOCK_SIZE] = "Hello";
    int offset = 2 + 11; // file1.txt starts at logical block 2
    fseek(disk, BLOCK_SIZE * offset, SEEK_SET); 
    fwrite(&data, sizeof(char), BLOCK_SIZE, disk);
}

// void readData(FILE *disk) {
//     char data[BLOCK_SIZE];
//     int offset = 3 + 11; // file1.txt starts at logical block 3
//     fseek(disk, BLOCK_SIZE * offset, SEEK_SET); 
//     fread(&data, sizeof(char), BLOCK_SIZE, disk);
//     printf("Data: %s\n", data);
// }

void readData(FILE *disk) {
    DirectoryEntry subDir[3]; // 2 folders, 1 file

	printf("Root Data Info:\n");
	fseek(disk, BLOCK_SIZE * 11, SEEK_SET);
    for (int i = 0; i < 3; i++) {
        fread(&subDir[i], sizeof(DirectoryEntry), 1, disk);
        printf("Data: %s.%s\n", subDir[i].filename, subDir[i].ext);
    }   

}

int main() {
    // Create disk
    FILE *disk = fopen("./disks/fake_disk_2.img", "wb");
    if (disk == NULL) {
        printf("Error creating disk image.\n");
        return 1;
    }

    writeSuperblock(disk);
    writeFAT(disk);
    writeBitmap(disk);
    writeRootDir(disk);
    writeSubDir(disk);
    writeData(disk);
    
    fclose(disk);
    printf("Hardcoded disk created successfully.\n");

    return 0;
}
