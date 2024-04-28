// filesystem.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "filesystem.h"
#include "fat.h"

#define FAT_ENTRY_COUNT 4096

superblock sb;
FATEntry FAT[FAT_ENTRY_COUNT];
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
char data_section[2097152]; // 1MB = 2048 blocks * 512 bytes = 1048576 bytes
int num_dir_per_block = BLOCK_SIZE / sizeof(DirectoryEntry); // 25 entries per block

// helper function to check access type
bool check_access(char* input) {
	int is_r = strcmp(input, "r");
	int is_w = strcmp(input, "w");
	int is_a = strcmp(input, "a");

	return (is_r == 0) || (is_w == 0) || (is_a == 0);
}

// helper function to compare filename
bool compare_filename(const char* filename, const DirectoryEntry* dir_entry) {
    // Combine filename and extension for comparison
    char full_filename[11];
    strncpy(full_filename, dir_entry->filename, 8);
    strncat(full_filename, ".", 1);
    strncat(full_filename, dir_entry->ext, 3);
	printf("full_filename: %s\n", full_filename);

    // Compare the full filename
    return strcmp(filename, full_filename) == 0;
}

// print a subdirectory
void print_subdir(DirectoryEntry* sub_dir) {
	printf("Subdirectory Info:\n");
	printf("Filename: %s\n", sub_dir->filename);
	printf("Extension: %s\n", sub_dir->ext);
	printf("First logical cluster: %d\n", sub_dir->first_logical_cluster);
	printf("File size: %d\n\n", sub_dir->file_size);
}

// Open a directory 
// Currently open in root directory
// Might fix to return a DIR struct instead of DirectoryEntry
DirectoryEntry* f_opendir(char* directory) {
	// look into FAT[0], find a DirectoryEntry with the same filename
	int bytes_count = 0;
	DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	
	while (bytes_count < BLOCK_SIZE) {
		// retrieve the first 20 bytes of FAT[0]
		memcpy(sub_dir, &FAT[0] + bytes_count, sizeof(DirectoryEntry));
		print_subdir(sub_dir);

		// check if the filename matches
		if (compare_filename(directory, sub_dir)) {
			printf("Directory opened: %s\n", directory);
			return sub_dir;
		} else {
			// move to the next DirectoryEntry
			bytes_count += sizeof(DirectoryEntry);
			printf("bytes_count: %d\n", bytes_count);
		}
	}

	return NULL;
}

// DirectoryEntry* f_opendir(char *directory) { // maybe make directory const char * since it shouldn't change?
//     int total_entries = num_dir_per_block; // get the number of directory entries total in FAT

// 	// using the FAT, read through all the entries
//     for (int i = 0; i < total_entries; i++) {
//         DirectoryEntry* dir_entry = &FAT[i];

// 		// checking if the dir_entry matches the directory name
//         if (strcmp(dir_entry->filename, directory) == 0) {
//             // return a copy of the DirectoryEntry
//             DirectoryEntry* dir_open = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));

//             if (dir_open == NULL) {
//                 printf("ERROR: Memory allocation failed.\n");
//                 return NULL;
//             }

//             memcpy(dir_open, dir_entry, sizeof(DirectoryEntry));
//             return dir_open;
//         }
//     }

//     // directory not found in the FAT
//     printf("ERROR: Directory "%s" could not be found.\n", directory);
//     return NULL;
// }

// Open a file, return a FileHandle 
// My attempt to open file1.txt 
// Will fix it so that it calls open_dir before opening a file
FileHandle* f_open(char* filename, char *access) {
  // check if access is valid
	if (!check_access(access)) {
		printf("Invalid access type.\n");
		return NULL;
	}
  
	// look into FAT[0], find a DirectoryEntry with the same filename
	int bytes_count = 0;
	DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	while (bytes_count < BLOCK_SIZE) {
		// retrieve the first 13 bytes of FAT[0]
		memcpy(sub_dir, &FAT[0] + bytes_count, sizeof(DirectoryEntry));
		print_subdir(sub_dir);

		// check if the filename matches
		if (compare_filename(filename, sub_dir)) {
			// create a new FileHandle
			FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
			file->abs_path = filename;
			file->file_desc = 0; // file descriptor
			file->access = access;
			file->position = 0;
			printf("File opened: %s\n", filename);

			return file;
		} else {
			// move to the next DirectoryEntry
			bytes_count += sizeof(DirectoryEntry);
			printf("bytes_count: %d\n", bytes_count);
		}
	}

	return NULL;
}

// Close a file
void f_close(FileHandle* file) {
	free(file);
	printf("File closed.\n");
}

// to be called before the mainloop
void fs_mount(char *diskname) {
	// extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func
	FILE *disk = fopen(diskname, "rb");

	//read + define superblock
	fseek(disk, 0, SEEK_SET);
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

	//read + define FAT
	fseek(disk, BLOCK_SIZE * sb.FAT_offset, SEEK_SET);
	fread(FAT, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk);
	printf("FAT Info:\n");
    for (int i = 0; i < FAT_ENTRY_COUNT; i++) {
        printf("%d ", FAT[i].block_number);
    }
    printf("\n");

	//read + define bitmap
	fseek(disk, BLOCK_SIZE * sb.FREEMAP_offset, SEEK_SET);
	fread(&bitmap, sizeof(BitmapBlock), 1, disk);
	printf("Bitmap Info:\n");
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%d", (bitmap.bitmap[i]));
    }
    printf("\n");

	//read + define rootdirentry
	fseek(disk, BLOCK_SIZE * sb.ROOTDIR_offset, SEEK_SET);
	fread(&root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 32 bytes
	printf("Root directory: %s\n", root_dir_entry.filename);

	//read + define datablock section
	fseek(disk, BLOCK_SIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
	fread(data_section, BLOCK_SIZE * sb.file_size_blocks, 1, disk); //Directory is size: 512 bytes
	printf("Data section: %s\n", data_section);

	fclose(disk);
}

// debug function to read the root directory
void read_dir(char *diskname) {
	FILE *disk = fopen(diskname, "rb");
	DirectoryEntry subDir[3]; // 2 folders, 1 file

	printf("Data Info:\n");
	fseek(disk, BLOCK_SIZE * 11, SEEK_SET);
    for (int i = 0; i < 3; i++) {
        fread(&subDir[i], sizeof(DirectoryEntry), 1, disk);
        printf("Data: %s\n", subDir[i].filename);
    }   

	fclose(disk);
}

int main(void) {
	// Mount the filesystem
	fs_mount("fake_disk.img");

	read_dir("fake_disk.img");
	
	// Open a file
	FileHandle* file = f_open("/file1.txt", "r");
	if (file == NULL) {
		printf("File not found.\n");
	} else {
		printf("File opened: %s\n", file->abs_path);
	}
}
