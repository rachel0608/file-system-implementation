typedef struct {
    uint8_t file_size_mb;       // file size in MB (default is 1MB)
    uint8_t FAT_offset;         // offset of first FAT block location
    uint8_t FREEMAP_offset;      // offset of first Free-Bitmap location
    uint8_t ROOTDIR_offset;     // offset of first ROOTDIR block location
    uint8_t DATA_offset;        // offset of first DATABLOCK location
    uint16_t block_size;        // block size will be standard 512 bytes
    uint32_t file_size_blocks;  // file size in blocks (1MB = 2048 blocks)
} superblock;

typedef struct {
    uint16_t block_number;  // 16-bit value (2 bytes) representing block number (after DATA_offset)
} FATEntry;

typedef struct {
    uint8_t bitmap[BLOCKSIZE]; // Bitmap to track free data blocks
} BitmapBlock;

typedef struct {
    char filename[9];
    char ext[3];
    uint16_t attributes;
    uint16_t reserved;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t ignored;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_block;
    uint16_t file_size; // 0 for directories
} DirectoryEntry; //32 bytes

typedef struct {
    DirectoryEntry entries[16];
} Directory; //holds 16 DirectoryEntrys

typedef struct {
    char value[BLOCKSIZE];
} datablock; //holds a block of data

//to be called before the mainloop
void fs_mount(char *diskname) {

    FILE *disk = fopen(diskname, "rb");
    printf("file opened\n");

    //extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func
    superblock sb;

    //read + define superblock
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(superblock), 1, disk);
    printf("-----------------------");
    printf("\nsb read and defined:\n");
    printf("Block Size = %d\n", sb.block_size);
    printf("File Size in MB = %d\n", sb.file_size_mb);
    printf("File Size in Blocks = %d\n", sb.file_size_blocks);
    printf("FAT Offset = %d\n", sb.FAT_offset);
    printf("FREEMAP Offset = %d\n", sb.FREEMAP_offset);
    printf("ROOTDIR Offset = %d\n", sb.ROOTDIR_offset);
    printf("DATA Offset = %d\n\n", sb.DATA_offset);

    int num_blocks = sb.file_size_blocks;
    FATEntry fat[num_blocks];

    //read + define FAT
    fseek(disk, BLOCKSIZE * sb.FAT_offset, SEEK_SET);
    fread(fat, (num_blocks * sizeof(FATEntry)), 1, disk);
    printf("---------------------------------------------------------");
    printf("\nfat read and defined: uncomment to print fat contents\n");
    for (int i = 0; i < num_blocks; i++) {
        //Uncomment to print fat contents
        //printf("fat cell %d: %d\n", i, fat[i].block_number); 
    }

    BitmapBlock bitmap;

    //read + define bitmap
    fseek(disk, BLOCKSIZE * sb.FREEMAP_offset, SEEK_SET);
    fread(&bitmap, sizeof(BitmapBlock), 1, disk);
    printf("-----------------------------------------------------------------");
    printf("\nbitmap read and defined: check cecilia's bitmap code for util\n");

    DirectoryEntry root_dir_entry;

    //read + define rootdirentry
    fseek(disk, BLOCKSIZE * sb.ROOTDIR_offset, SEEK_SET);
    fread(&root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 32 bytes
    printf("-----------------------------------");
    printf("\nroot dir entry read and defined:\n");
    char filename[9];
    char ext[3];
    uint16_t attributes;
    uint16_t reserved;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t ignored;
    uint16_t last_write_time;
    uint16_t last_write_date;
    uint16_t first_block;
    uint16_t file_size; // 0 for directories
    printf("filename: %s\n", root_dir_entry.filename);
    printf("ext: %s\n", root_dir_entry.ext);
    printf("attributes: %d\n", root_dir_entry.attributes);
    printf("reserved: %d\n", root_dir_entry.reserved);
    printf("creation time: %d\n", root_dir_entry.creation_time);
    printf("creation date: %d\n", root_dir_entry.creation_date);
    printf("last access date: %d\n", root_dir_entry.last_access_date);
    printf("ignored: %d\n", root_dir_entry.ignored);
    printf("last write time: %d\n", root_dir_entry.last_write_time);
    printf("last write date: %d\n", root_dir_entry.last_write_date);
    printf("first block: %d\n", root_dir_entry.first_block);
    printf("file size: %d\n", root_dir_entry.file_size);

    datablock data_section[sb.file_size_blocks];

    //read + define datablock section
    fseek(disk, BLOCKSIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
    fread(data_section, BLOCKSIZE * sb.file_size_blocks, 1, disk); //Directory is size: 512 bytes
    printf("--------------------------------------------------");
    printf("\ndata section read and defined: currently empty\n");

    fclose(disk);
}
