// Root directory is also a directory entry
typedef struct {
    char filename[8];
    char ext[3];
    uint8_t attributes;
    uint16_t reserved;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t ignored;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_logical_cluster;   // 0 for root, 1 reserved, first data cluster is 2
    uint32_t file_size; // 0 for directories
} DirectoryEntry;

typedef struct {
    uint32_t content;
} FatEntry;

// find the directory entry for a file 
DirectoryEntry* find_file(char* filename);

// read file contents from clusters
void fat_read_file_contents(FatEntry *start_cluster, uint32_t file_size, uint8_t buffer);

// allocate a chain of clusters for a new file
void fat_allocate_cluster_chain(FatEntry* start_cluster, uint32_t file_size);

// update directory entry for a file   
void fat_update_directory_entry(const char* filename, FatEntry *start_cluster, uint32_t file_size);

// free clusters allocated to a file
void fat_free_cluster_chain(FatEntry *start_cluster);
