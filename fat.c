// find the directory entry for a file 
DirectoryEntry* find_file(char* filename){
    // Traverse through directory entries and compare filenames
    // Return pointer to the directory entry if found, NULL otherwise
 }

// read file contents from start cluster
void fat_read_file_contents(FatEntry *start_cluster, uint32_t file_size, uint8_t buffer) {
    // Start from the given start_cluster and read clusters until file_size is reached
    // Copy the file data into the provided buffer
}

// allocate a chain of clusters for a new file
void fat_allocate_cluster_chain(FatEntry* start_cluster, uint32_t file_size) {
    // Find free clusters in the FAT
    // Update FAT entries to create a cluster chain for the file
    // Update start_cluster with the first cluster of the allocated chain
}

// update directory entry for a file
void fat_update_directory_entry(const char* filename, FatEntry *start_cluster, uint32_t file_size) {
    // Find the directory entry corresponding to the filename
    // Update the entry with the new start cluster and file size
}

// free clusters allocated to a file
void fat_free_cluster_chain(FatEntry *start_cluster) {
    // Update FAT entries to mark clusters as free starting from start_cluster
}
