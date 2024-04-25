/*
Layout: superblock (1) | FAT (8) | FREEMAP (1) | ROOTDIR (1) | DATA

FAT[0] is reserved for root directory
FAT[1] is folder1 (empty)
FAT[2] is folder2 (empty)
FAT[3] is file1.txt
*/ 

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 512
#define FAT_SIZE 8 // in blocks
#define ROOT_DIR_SIZE 1

typedef struct {
    uint8_t file_size_mb;
    uint8_t FAT_offset;
    uint8_t FREEMAP_offset;
    uint8_t ROOTDIR_offset;
    uint8_t DATA_offset;
    uint16_t block_size;
    uint32_t file_size_blocks;
} superblock;

typedef struct {
    uint16_t block_number;
} FATEntry;

typedef struct {
    uint8_t bitmap[BLOCK_SIZE];
} BitmapBlock;

typedef struct {
    char filename[8+1];
    char ext[3+1];
    uint16_t first_logical_cluster;
    uint32_t file_size;
} DirectoryEntry;

void writeSuperblock(FILE *disk) {
    superblock sb;
    sb.file_size_mb = 1;
    sb.FAT_offset = 1;
    sb.FREEMAP_offset = 9;
    sb.ROOTDIR_offset = 10;
    sb.DATA_offset = 12;
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

    // Folder1 and Folder2 are empty
    FAT[1].block_number = 0; // EOF
    FAT[2].block_number = 0; // EOF

    // Allocate blocks for file1.txt  
    FAT[3].block_number = 4; // file1.txt
    FAT[4].block_number = 0; // EOF

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

    // Mark blocks 1-4 as used
    for (int i = 1; i <= 4; i++) {
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
    
    strcpy(subDir[0].filename, "folder1");
    strcpy(subDir[0].ext, "");
    subDir[0].first_logical_cluster = 1;
    subDir[0].file_size = 0;

    strcpy(subDir[1].filename, "folder2");
    strcpy(subDir[1].ext, "");
    subDir[1].first_logical_cluster = 2;
    subDir[1].file_size = 0;

    strcpy(subDir[2].filename, "file1");
    strcpy(subDir[2].ext, "txt");
    subDir[2].first_logical_cluster = 3;
    subDir[2].file_size = 5;

    fseek(disk, BLOCK_SIZE * 11, SEEK_SET); // Subdirectories start at block 11
    fwrite(&subDir, sizeof(DirectoryEntry), 3, disk);
}

void writeData(FILE *disk) {
    char data[BLOCK_SIZE] = "Hello";
    int offset = 3 + 11; // file1.txt starts at logical block 3
    fseek(disk, BLOCK_SIZE * offset, SEEK_SET); 
    fwrite(&data, sizeof(char), BLOCK_SIZE, disk);
}

void readData(FILE *disk) {
    char data[BLOCK_SIZE];
    int offset = 3 + 11; // file1.txt starts at logical block 3
    fseek(disk, BLOCK_SIZE * offset, SEEK_SET); 
    fread(&data, sizeof(char), BLOCK_SIZE, disk);
    printf("Data: %s\n", data);
}

int main() {
    // Create disk
    FILE *disk = fopen("fake_disk.img", "wb");
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

    // Read disk
    disk = fopen("fake_disk.img", "rb");

    readSuperblock(disk);
    readFAT(disk);
    readBitmap(disk);
    readRootDir(disk);
    readData(disk);

    fclose(disk);

    printf("Hardcoded disk read successfully.\n");
    return 0;
}
