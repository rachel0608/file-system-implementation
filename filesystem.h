typedef struct {
    uint16_t bytes_per_sector;      // Bytes per sector (512)
    uint8_t sectors_per_cluster;    // Sectors per cluster (powers of 2 from 1-128)
    uint16_t total_sectors;         // Total number of sectors 
    uint16_t sectors_per_fat;       // Sectors per FAT
    uint16_t sectors_per_track;     // Sectors per track
    uint16_t num_heads;             // Number of heads
    uint32_t hidden_sectors;        // Number of hidden sectors before this partition
    uint32_t total_sectors_big;     // Total number of sectors (if total_sectors is 0)
} BootSector;

typedef struct {
  char* abs_path; // Absolute path
  int file_desc; // File descriptor
  char* access; // File access type
  int position; // File position
  int group_id;
  int owner_id;
} FileHandle; // FileHandle loosely based on Unix systems’s FileHandle (may not need?)

typedef struct {
    char* username;     // Username
    char* password;     // Password
    int user_id;        // User ID (super user = 0 or regular user)
} User;

typedef struct {
    char* groupname;    // Group name
    int group_id;       // Group ID
    int* member_ids;    // Array of user IDs who are members of this group
    int num_members;    // Number of members in the group
} Group;

// tEchNicaLLy filename = directory = path

FileHandle* f_open(char* filename, char* access, User* user) // access = w/w+, r/r+, a/a+ maybe add group later as param
int f_read(FileHandle file, void* buffer, size_t bytes);
int f_write(FileHandle file, void* buffer, size_t bytes);
void f_close(FileHandle file, User* user);
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
