// filesystem.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "filesystem.h"
#include "fat.h"

superblock sb;
FATEntry *FAT;
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
Directory *root_dir;
datablock *data_section; // 1MB = 2048 blocks * 512 bytes = 1048576 bytes
int num_dir_per_block = BLOCK_SIZE / sizeof(DirectoryEntry); // 26 entries per block

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
    // strncat(full_filename, ".", 1);
	strcat(full_filename, ".");
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

FileHandle* f_open(char* path, char* access) {
	if (!check_access) {
		printf("ERROR: Invalid access type.\n");
		return NULL;
	}

	DirectoryEntry* open_entry = f_opendir(path);

	if (open_entry == NULL) {
		printf("ERROR: File at this path not found - %s\n", path);
		return NULL;
	}

	FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
	file->abs_path = path;
	file->file_desc = 0;  // Default for now, can change, just put 0
	file->access = access;
	file->position = 0;

	return file;
}

/* int f_read(FileHandle file, void* buffer, size_t bytes) {
	// Checking if the file can be opened for reading
	if ((strcmp(file.access, "r") != 0) && (strcmp(file.access, "r+") != 0) && (strcmp(file.access, "a+") != 0)) {
        printf("ERROR: Cannot open file for reading.\n");
        return -1;
    }

	// Maybe change find_file to opendir??
    DirectoryEntry *dir_entry = find_file(file->abs_path);
    if (dir_entry == NULL) {
        printf("ERROR: File at this path not found - %s\n", file->abs_path);
        return -1;
    }

	FATEntry start_cluster;
    start_cluster.block_number = dir_entry->first_logical_cluster;
    fat_read_file_contents(&start_cluster, dir_entry->file_size, buffer);

	file->position += bytes;
	return bytes;
}
*/

// int f_write(FileHandle file, void* buffer, size_t bytes);

// Open a directory 
// Currently open in root directory
// Might fix to return a DIR struct instead of DirectoryEntry
DirectoryEntry* f_opendir(char* directory) {
	// look into FAT[0], find a DirectoryEntry with the same filename
	int bytes_count = 0;
	DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	
	while (bytes_count < BLOCK_SIZE) {
		// retrieve the first 20 bytes of FAT[0]
		memcpy(sub_dir, root_dir + bytes_count, sizeof(DirectoryEntry));
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

DirectoryEntry* f_readdir(char* path) {

}

// Open a file, return a FileHandle 
// My attempt to open file1.txt 
// Will fix it so that it calls open_dir before opening a file
// FileHandle* f_open(char* filename, char *access) {
//   // check if access is valid
// 	if (!check_access(access)) {
// 		printf("Invalid access type.\n");
// 		return NULL;
// 	}
  
// 	// look into FAT[0], find a DirectoryEntry with the same filename
// 	int bytes_count = 0;
// 	DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
// 	while (bytes_count < BLOCK_SIZE) {
// 		// retrieve the first 13 bytes of FAT[0]
// 		memcpy(sub_dir, &FAT[0] + bytes_count, sizeof(DirectoryEntry));
// 		print_subdir(sub_dir);

// 		// check if the filename matches
// 		if (compare_filename(filename, sub_dir)) {
// 			// create a new FileHandle
// 			FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
// 			file->abs_path = filename;
// 			file->file_desc = 0; // file descriptor
// 			file->access = access;
// 			file->position = 0;
// 			printf("File opened: %s\n", filename);

// 			return file;
// 		} else {
// 			// move to the next DirectoryEntry
// 			bytes_count += sizeof(DirectoryEntry);
// 			printf("bytes_count: %d\n", bytes_count);
// 		}
// 	}

// 	return NULL;
// }

// // Close a file
// void f_close(FileHandle* file) {
// 	free(file);
// 	printf("File closed.\n");
// }


// to be called before the mainloop
void fs_mount(char *diskname) {

	// extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func
	FILE *disk = fopen(diskname, "rb");
	if (disk == NULL) {
	    fprintf(stderr, "Error: Failed to open disk file '%s'\n", diskname);
	    exit(EXIT_FAILURE);
	}
	
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
    FAT = (FATEntry *)malloc(sb.file_size_blocks * sizeof(FATEntry));
	fseek(disk, BLOCK_SIZE * sb.FAT_offset, SEEK_SET);
	fread(FAT, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk);
	printf("FAT Info:\n");
	for (int i = 0; i < 10; i++) {
	    //replace 10 with sb.file_size_blocks to print entire data section
        printf("fat cell %d: %d\n", i, FAT[i].block_number); 
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
	fread(&root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 19 bytes
	printf("Root directory: %s\n", root_dir_entry.filename);
	printf("first block: %d\n", root_dir_entry.first_logical_cluster);
    printf("file size: %d\n", root_dir_entry.file_size);
    printf("file type: %d\n", root_dir_entry.type);
	printf("\n");
	
	//read + define datablock section
    data_section = (datablock *)malloc(sb.file_size_blocks * sizeof(datablock));
	fseek(disk, BLOCK_SIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
	fread(data_section, BLOCK_SIZE, sb.file_size_blocks, disk); //Directory is size: 512 bytes
	printf("Data section: \n");
    for (int i = 0; i < 10; i++) {
        //replace 10 with sb.file_size_blocks to print entire data section
        printf("block %d: %s\n", i, data_section[i].buffer);
    }
    printf("\n");

    // Access the first block in the data section to verify the copy
    root_dir = (Directory *) data_section[0].buffer;
    printf("Root dir's first entry filename: %s\n", root_dir->entries[0].filename);
    printf("Root dir's first entry file size: %d\n", root_dir->entries[0].file_size);
    printf("Root dir's first entry first block: %d\n", root_dir->entries[0].first_logical_cluster);
    printf("Root dir's first entry type: %d\n", root_dir->entries[0].type);
	
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
	fs_mount("fake_disk_3_folders.img");

	// read_dir("fake_disk.img");
	//f_opendir("fake_disk.img");
	
	// Open a file
}
