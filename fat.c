/**
 * fat.c
 * Authors: czhang, gchoe, jrieger, rnguyen
 * Date: 5/11/2024
 * 
 * Description:
 * Implementation of FAT.
 */

#include "fat.h"
#include "filesystem.h"
#include <string.h>

// Find the directory entry for a file
DirectoryEntry* find_file(char* filename){
    // Traverse through directory entries in the root directory
    for (int i = 0; i < (BLOCK_SIZE / sizeof(DirectoryEntry)); i++) {
        // Get the directory entry at index i
        DirectoryEntry entry = root_dir->entries[i];
        // Check if the filename matches
        if (strcmp(entry.filename, filename) == 0) {
            // Return a pointer to the matching directory entry
            return &entry;
        }
    }
    // If the file is not found, return NULL
    return NULL;
}

// read file contents from start cluster
void fat_read_file_contents(FATEntry *start_cluster, uint32_t file_size, uint8_t *buffer) {
    // Initialize variables for tracking the current cluster and buffer offset
    FATEntry current_cluster = *start_cluster;
    uint32_t bytes_read = 0;

    // Read file data from clusters until the entire file is read
    while (bytes_read < file_size) {
        // Calculate the block number corresponding to the current cluster
        uint16_t block_number = current_cluster.block_number;

        // Determine how many bytes to read from the current block
        uint32_t bytes_to_read = (file_size - bytes_read < BLOCK_SIZE) ? (file_size - bytes_read) : BLOCK_SIZE;

        // Read data from the block using f_read and copy it into the buffer
        int result = f_read(&data_section[block_number], buffer + bytes_read, bytes_to_read);
        if (result < 0) {
            // Handle error
            // For simplicity, we're just printing an error message here
            printf("Error reading file data\n");
            return;
        }

        // Update bytes read and move to the next cluster
        bytes_read += bytes_to_read;
        current_cluster = FAT[block_number];
    }
}

// allocate a chain of clusters for a new file
void fat_allocate_cluster_chain(FATEntry* start_cluster, uint16_t start_cluster_idx, uint32_t file_size) {
    // Initialize variables
    FATEntry prev_cluster = *start_cluster;
    uint32_t bytes_allocated = BLOCK_SIZE;

    // Iterate through the FAT to find free clusters and allocate them
    for (uint16_t i = start_cluster_idx+1; i < sb.file_size_blocks; i++) {
        // Check if the current cluster is free
        if (FAT[i].block_number == FREEBLOCK) {
            //set previous cluster block number to curr cluster index
            prev_cluster.block_number = i;

            // Update the current cluster to link it to the next one
            prev_cluster = FAT[i];

            // Update the number of bytes allocated
            bytes_allocated += BLOCK_SIZE;

            // Check if we've allocated enough clusters for the file
            if (bytes_allocated >= file_size) {
                FAT[i].block_number = END_OF_FILE;
                return; // Allocation complete
            }
        }
    }
}

// update directory entry for a file
void fat_update_directory_entry(const char* filename, FATEntry *start_cluster, uint32_t file_size) {
    // Find the directory entry corresponding to the filename
    // Update the entry with the new start cluster and file size
}

// free clusters allocated to a file
void fat_free_cluster_chain(FATEntry *start_cluster) {
    // Update FAT entries to mark clusters as free starting from start_cluster
}
