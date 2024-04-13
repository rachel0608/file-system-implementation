#include <stdio.h>
#include <stdlib.h>

#define DISK_SIZE_MB_DEFAULT 1
#define FILENAME argv[1]
#define FLAG argv[2]
#define FLAG_PRESENT 4

typedef struct {
    // Define FAT entry structure for FAT-12
    uint16_t fat_entries[NUM_BLOCKS]; // array representing each block on the disk
} FATEntry;

typedef struct {
    // Define MFT entry structure for storing file metadata
    uint8_t filename[16];   // File name (max 16 characters)
    uint32_t file_size;     // File size in bytes
    uint16_t start_block;   // Starting datablock of file data
    uint8_t attributes;     // File attributes (read-only, hidden, ...?)
} MFTEntry;

int main(int argc, char *argv[]) {
    char *filename;
    int disk_size_mb = DISK_SIZE_MB_DEFAULT;
 
    // Parse command-line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [-s #]\n", argv[0]);
        return EXIT_FAILURE;
    }

    filename = FILENAME;

    if (argc == FLAG_PRESENT && strcmp(FLAG, "-s") == 0) {
        disk_size_mb = atoi(argv[3]);
    }

    // Create the disk image file
    FILE *disk_image = fopen(filename, "wb");
    if (!disk_image) {
        perror("Failed to create disk image file");
        return EXIT_FAILURE;
    }

    // Initialize disk image with FAT and MFT structures
    initialize_disk_image(disk_image, disk_size_mb);

    // Close the disk image file
    fclose(disk_image);

    printf("Disk image formatted successfully: %s\n", filename);
    return EXIT_SUCCESS;
}

void initialize_disk_image(FILE *disk_image, int disk_size_mb) {
    // Initialize FAT
    initialize_fat(disk_image, disk_size_mb);

    // Initialize MFT (or file system metadata)
    initialize_mft(disk_image);
}

void initialize_fat(FILE *disk_image, int disk_size_mb) {
    // Calculate number of clusters based on disk size (assume cluster size)
    int num_clusters = disk_size_mb * 1024;  // Assuming 1 cluster = 1 KB

    // Allocate memory for FAT entries
    FATEntry *fat_entries = (FATEntry *)malloc(num_clusters * sizeof(FATEntry));
    if (!fat_entries) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Initialize FAT entries (e.g., set initial cluster allocation status)
    // Example:
    // for (int i = 0; i < num_clusters; i++) {
    //     fat_entries[i].cluster_status = CLUSTER_FREE; // Initialize to free
    // }

    // Write FAT entries to disk image file
    fwrite(fat_entries, sizeof(FATEntry), num_clusters, disk_image);

    // Free allocated memory
    free(fat_entries);
}

void initialize_mft(FILE *disk_image) {
    // Initialize MFT entries (e.g., for system files and metadata)
    // Example:
    // MFTEntry root_directory_entry;
    // strcpy(root_directory_entry.filename, "ROOT");
    // root_directory_entry.file_size = 0; // Root directory size initially 0
    // root_directory_entry.start_cluster = FIRST_CLUSTER_ROOT_DIR;
    // root_directory_entry.attributes = ATTR_DIRECTORY;

    // Write MFT entries to disk image file
    // fwrite(&root_directory_entry, sizeof(MFTEntry), 1, disk_image);
    // Write other necessary MFT entries (system files, etc.)
}
