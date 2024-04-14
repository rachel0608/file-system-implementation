#include <stdio.h>
#include <stdlib.h>

#define DISK_SIZE_MB_DEFAULT 1
#define EXPECTED_ARGS 2
#define EXECUTABLE argv[0]
#define FILENAME argv[1]
#define FLAG argv[2]
#define FLAG_PRESENT 4
#define FLAG_PARAM argv[3]

typedef struct {
    uint16_t value;  // 12-bit value representing allocation status or block number
} FATEntry;

typedef struct {
    // Define MFT entry structure for storing file metadata
    uint8_t filename[MAX_NAME];   // File name
    uint32_t file_size;           // File size in bytes
    uint16_t start_block;         // Starting datablock of file data
    uint8_t attributes;           // File attributes (read-only, hidden, ...?)
} MFTEntry;

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

    // Initialize disk image with FAT and MFT structures
    initialize_disk_image(disk_image, disk_size_mb);

    fclose(disk_image);

    printf("Disk image formatted successfully: %s\n", filename);
    return EXIT_SUCCESS;
}

void initialize_disk_image(FILE *disk_image, int disk_size_mb) {
    initialize_fat(disk_image, disk_size_mb);
    initialize_mft(disk_image);
}

void initialize_fat(FILE *disk_image, int disk_size_mb) {
    // Calculate number of blocks based on disk size
    int num_blocks = disk_size_mb * 1000;  // Assuming 1 block = 1 KB

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
    fwrite(fat_entries, num_blocks * sizeof(FATEntry), num_blocks, disk_image);

    // Free allocated memory
    free(fat_entries);
}

void initialize_mft(FILE *disk_image) {
    // Initialize MFT entries (for system files and metadata)
    MFTEntry root_directory_entry;
    strcpy(root_directory_entry.filename, "ROOT");
    root_directory_entry.file_size = 0; // Root directory size initially 0
    root_directory_entry.start_cluster = FIRST_BLOCK_ROOT_DIR;
    root_directory_entry.attributes = DIR_ATTR;

    // Write MFT entries to disk image file
    fwrite(&root_directory_entry, sizeof(MFTEntry), 1, disk_image);
    
    // Write other MFT entries? (system files)
}
