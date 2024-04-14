#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

#define DISK_SIZE_MB_DEFAULT 1
#define EXPECTED_ARGS 2
#define EXECUTABLE argv[0]
#define FILENAME argv[1]
#define FLAG argv[2]
#define FLAG_PRESENT 4
#define FLAG_PARAM argv[3]
#define MAX_NAME 8

int main(int argc, char *argv[]) {
 
    // Parse command-line arguments
    if (argc < EXPECTED_ARGS) {
        fprintf(stderr, "Usage: %s <filename> -s <num MB requested>\n", EXECUTABLE);
        return EXIT_FAILURE;
    }

    int disk_size_mb = DISK_SIZE_MB_DEFAULT;
    char *filename = FILENAME;

    if (argc == FLAG_PRESENT && strcmp(FLAG, "-s") == 0) {
        disk_size_mb = atoi(FLAG_PARAM);
    }

    // Open disk to format
    FILE *disk_image = fopen(filename, "wb"); 
    if (!disk_image) {
        perror("Failed to create disk image file");
        return EXIT_FAILURE;
    }

    //determine/allocate appropriate size for disk image?

    // Initialize disk image with FAT and MFT structures
    initialize_disk_image(disk_image, disk_size_mb);

    fclose(disk_image);

    printf("Disk image formatted successfully: %s\n", filename);
    return EXIT_SUCCESS;
}

void initialize_disk_image(FILE *disk_image, int disk_size_mb) {
    initialize_fat(disk_image, disk_size_mb);
    initialize_mft(disk_image);
    initialize_rootdir(); //init directoryEntry for rootdir
    initialize_bootsector(); //init boot block and superblock
}

void initialize_fat(FILE *disk_image, int disk_size_mb) {
    // Calculate number of blocks based on disk size
    int num_blocks = disk_size_mb * 1024;  // Assuming 1 block = 512 bytes

    // Allocate memory for FAT entries
    FATEntry *fat_entries = (FATEntry *)malloc(num_blocks * sizeof(FATEntry));
    if (!fat_entries) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Initialize FAT entries
    // Assume all blocks are initially free
    for (int i = 0; i < num_blocks; i++) {
        if (i == 0 || i == 1) {
            // Reserved blocks (boot block, superblock)
            fat_entries[i].value = 0xFFF;  // Mark as reserved (end of file)
        } else {
            // Free cluster
            fat_entries[i].value = 0x000;  // Mark as free
        }
    }

    // Write FAT entries to disk image file
    // Where should we write the FAT in the disk???
    fwrite(fat_entries, num_blocks * sizeof(FATEntry), num_blocks, disk_image);

    // Free allocated memory
    free(fat_entries);
}

void initialize_mft(FILE *disk_image) {
    // Initialize MFT entries (for system files and metadata)
    FileHandle root_directory_entry;
    strcpy(root_directory_entry.filename, "ROOT");
    root_directory_entry.file_size = 0; // Root directory size initially 0
    root_directory_entry.start_cluster = FIRST_BLOCK_ROOT_DIR;
    root_directory_entry.attributes = DIR_ATTR;

    // Write MFT entries to disk image file
    // Where should we write the MFT in the disk???
    fwrite(&root_directory_entry, sizeof(FileHandle), 1, disk_image);
    
    // Write other MFT entries? (system files)
}

void initialize_rootdir(FILE *disk_image) {
    // Initialize the root directory entry
    DirectoryEntry root_entry;
    strcpy(root_entry.filename, ".");
    strcpy(root_entry.ext, "");
    root_entry.attributes = DIR_ATTR; // Assuming directory attribute
    root_entry.creation_time = 0;     // Initialize creation time
    root_entry.creation_date = 0;     // Initialize creation date
    root_entry.last_access_date = 0;  // Initialize last access date
    root_entry.ignored = 0;           // Reserved field
    root_entry.last_write_time = 0;   // Initialize last write time
    root_entry.last_write_date = 0;   // Initialize last write date
    root_entry.first_logical_cluster = FIRST_BLOCK_ROOT_DIR; // First cluster of the root directory
    root_entry.file_size = 0;         // Root directory size (0 for directories)

    // Write the root directory entry to the disk image
    fseek(disk_image, FIRST_BLOCK_ROOT_DIR * BLOCK_SIZE, SEEK_SET);
    fwrite(&root_entry, sizeof(DirectoryEntry), 1, disk_image);
}

void initialize_bootsector(FILE *disk_image, int disk_size_mb) {
    // Initialize the boot sector
    BootSector boot_sector;
    boot_sector.bytes_per_sector = BLOCK_SIZE; // Assuming block size (e.g., 512 bytes)
    boot_sector.sectors_per_cluster = SECTORS_PER_CLUSTER; // Sectors per cluster
    boot_sector.reserved_sector_count = RESERVED_SECTOR_COUNT; // Reserved sectors
    boot_sector.fat_count = FAT_COUNT; // Number of FAT tables
    boot_sector.root_dir_entry_count = ROOT_DIR_ENTRY_COUNT; // Number of root directory entries
    boot_sector.total_sectors_16 = disk_size_mb * 1024; // Total number of sectors in the disk
    boot_sector.media_type = MEDIA_TYPE; // Media type (e.g., fixed disk)
    boot_sector.sectors_per_fat = SECTORS_PER_FAT; // Sectors per FAT
    boot_sector.sectors_per_track = SECTORS_PER_TRACK; // Sectors per track
    boot_sector.heads = HEADS; // Number of heads
    boot_sector.hidden_sectors = HIDDEN_SECTORS; // Hidden sectors
    boot_sector.total_sectors_32 = 0; // Total sectors for disks > 32 MB (0 for now)

    // Write the boot sector to the disk image
    fseek(disk_image, 0, SEEK_SET);
    fwrite(&boot_sector, sizeof(BootSector), 1, disk_image);
}

void initialize_bootsector(FILE *disk_image, int disk_size_mb) {
    BootSector boot_sector;

    // Init boot sector structure
    memset(&boot_sector, 0, sizeof(BootSector));
    boot_sector.bytes_per_sector = 512;
    boot_sector.sectors_per_cluster = 1;
    boot_sector.reserved_sector_count = 1;
    boot_sector.num_fats = 2;
    boot_sector.root_entry_count = 512;
    boot_sector.total_sectors = disk_size_mb * 1024 / boot_sector.bytes_per_sector;
    boot_sector.media_descriptor = 0xF8;
    boot_sector.sectors_per_fat = 9;  // Assuming 9 sectors per FAT for FAT-12
    boot_sector.sectors_per_track = 18;
    boot_sector.num_heads = 2;
    boot_sector.total_sectors_big = 0; // Not used for FAT-12

    // Write the boot sector structure at the beginning of the disk image file
    fseek(disk_image, 0, SEEK_SET);
    fwrite(&boot_sector, sizeof(BootSector), 1, disk_image);
}
