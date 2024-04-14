typedef struct {
    uint8_t jmp_instruction[3];     // JMP instruction to boot code
    char oem_name[8];               // OEM name and version
    uint16_t bytes_per_sector;      // Bytes per sector (512)
    uint8_t sectors_per_cluster;    // Sectors per cluster (powers of 2 from 1-128)
    uint16_t reserved_sector_count; // Number of reserved sectors (1 for FAT-12)
    uint8_t num_fats;               // Number of FATs (2: 1 origin + 1 copy)
    uint16_t root_entry_count;      // Number of root directory entries 
    uint16_t total_sectors;         // Total number of sectors 
    uint8_t media_descriptor;       // Media descriptor
    uint16_t sectors_per_fat;       // Sectors per FAT
    uint16_t sectors_per_track;     // Sectors per track
    uint16_t num_heads;             // Number of heads
    uint32_t hidden_sectors;        // Number of hidden sectors before this partition
    uint32_t total_sectors_big;     // Total number of sectors (if total_sectors is 0)
} BootSector;


typedef struct {
  char* filename; // Filename
  int file_desc; // File descriptor
  char* access; // File access type
  int position; // File position
} FileHandle; // FileHandle loosely based on Unix systems’s FileHandle

// tEchNicaLLy filename = directory = path

FileHandle* f_open(char* filename, char* access) // access = w/w+, r/r+, a/a+
int f_read(FileHandle file, void* buffer, size_t bytes);
int f_write(FileHandle file, void* buffer, size_t bytes);
void f_close(FileHandle file);
int f_seek(FileHandle file, long offset, int whence);
void f_rewind(FileHandle file);
int f_stat(FileHandle file, struct stat *buffer);
int f_remove(const char* filename);
DirectoryEntry* f_opendir(char* directory);
DirectoryEntry* f_readdir(char* directory);
Int f_closedir(char* path);
int f_mkdir(char* path);
int f_rmdir(char* path);

f_mount(); // Extra credit
f_umount(); // Extra credit

// Pointers to heads of free lists
DirectoryEntry* freeHeadDE = NULL;
FatEntry* freeHeadFE = NULL;
