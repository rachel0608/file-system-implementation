/**
 * filesystem.c
 * Authors: czhang, gchoe, jrieger, rnguyen
 * Date: 5/11/2024
 * 
 * Description:
 * Implementation of the filesystem.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "filesystem.h"
#include "fat.h"

// moved to filesystem.h
// #define SEEK_SET 0
// #define SEEK_CUR 1
// #define SEEK_END 2
// #define MAX_DIRS 100
// #define EMPTY 65535 // first_logical_cluster value for empty directory
// #define END_OF_FILE 0 // FAT entry value for end of file

FileHandle* opened_dirs[MAX_DIRS] = { NULL }; // All files that are open

// define the extern variables declared in fat.h
/* superblock sb;
FATEntry *FAT = NULL;
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
Directory *root_dir = NULL;
datablock *data_section = NULL;  */// 1MB = 2048 blocks * 512 bytes = 1048576 bytes

int num_dir_per_block = BLOCK_SIZE / sizeof(DirectoryEntry); // 26 entries per block

// helper function to compare filename
bool compare_filename(const char* filename, DirectoryEntry* dir_entry) {
    if (dir_entry->type == 1) {
		return strcmp(filename, dir_entry->filename) == 0;
    }

    // else combine filename and extension for comparison
    char full_filename[12];
    strncpy(full_filename, dir_entry->filename, 8);
	strcat(full_filename, ".");
    strncat(full_filename, dir_entry->ext, 3);
	// printf("full_filename: %s\n", full_filename);

    // Compare the full filename
    return strcmp(filename, full_filename) == 0;
}

bool compare_cluster(const char* filename, const DirectoryEntry* dir_entry) {
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
	if (strcmp(sub_dir->filename, "") != 0) {
		printf("Subdirectory Info:\n");
		printf("Filename: %s\n", sub_dir->filename);
		printf("Extension: %s\n", sub_dir->ext);
		if (sub_dir->first_logical_cluster == EMPTY){
			printf("First logical cluster: %d (empty)\n", sub_dir->first_logical_cluster);
		} else{
			printf("First logical cluster: %d\n", sub_dir->first_logical_cluster);
		}
	}
}

// print all dir entries in a directory
void print_dir(Directory* entry) {
	for(int i = 0; i < num_dir_per_block; i++){
		print_subdir(&entry->entries[i]);
	}
}

// print a subdirectory
void print_file_handle(FileHandle* file) {
	if (file != NULL) {
		printf("FileHandle Info:\n");
		printf("Absolute Path: %s\n", file->abs_path);
		printf("First Logical Cluster: %d\n", file->first_logical_cluster);
		printf("File Descriptor: %d\n", file->file_desc);
		printf("File Access Type: %s\n", file->access);
		printf("File Position: %d\n", file->position);
		printf("File Size: %d\n\n", file->file_size);
	}
}

void reformat_path(char* path) { // removes the file from dir path
	if (path == NULL || *path == '\0') {
        printf("Invalid input: NULL pointer or empty string.\n");
        return;
    }
    char *last_slash = strrchr(path, '/'); // Find last occurrence of ‘/’
    if (last_slash != NULL) {
        *last_slash = '\0'; // Truncate the string at the last ‘/’
    } else {
        printf("No ‘/’ found in the string.\n");
    }

	strcat(path, "/");
}

char* find_relative_path(char* path) { // last part of the path
	if (path == NULL || *path == '\0') {
		printf("Invalid input: NULL pointer or empty string.\n");
		return NULL;
	}
	char *last_slash = strrchr(path, '/'); // Find last occurrence of ‘/’
	if (last_slash != NULL) {
		return last_slash + 1; // Return the string after the last ‘/’
	} else {
		printf("No ‘/’ found in the string.\n");
		return path;
	}
}

char *extract_filename(const char *path) {
    // Find the last occurrence of '/' in the path
    const char *last_slash = strrchr(path, '/');

    // If no slash found, return the original path
    if (last_slash == NULL) {
        return strdup(path);
    }

    // Return a pointer to the character after the last slash
    return strdup(last_slash + 1);
}

FileHandle* f_open(char* path, char* access) { 
	// Call fopendir, then get the return value of fopendir and feed it into freaddir
	// then get the return value and go through these directory structs and find the
	// path specified/ compare filenames
	if ((strcmp(access, "r") == 0) && (strcmp(access, "w") == 0) && (strcmp(access, "a") == 0) &&
		(strcmp(access, "r+") == 0)) {
		printf("ERROR: Invalid access type.\n");
		return NULL;
	}

	// check user permission -- if user can R, then cannot W
	// if user cannot R or W, then just return null
	// For now not done since we don't have user type id

	// parse path first -- path included file names. (/home/a.txt --> /home)
	char new_path[2048];
	strncpy(new_path, path, sizeof(new_path) - 1);
	reformat_path(new_path);
	char* filename = find_relative_path(path);

	DirectoryEntry* open_entry = f_opendir(new_path);
	Directory* all_dirs = f_readdir(open_entry);
	DirectoryEntry* target_file = NULL;

	for (int i = 0; i < num_dir_per_block; i++) {
		// check if the filename matches		
		if (compare_filename(filename, &all_dirs->entries[i])) {
			target_file = &all_dirs->entries[i];
			printf("File opened: %s\n", open_entry->filename);
		}
	}

	if (target_file == NULL) {
		printf("ERROR: File does not exist.\n");
		return NULL;
	}

	if (target_file->type == 1) {
		printf("ERROR: Cannot open a directory.\n");
		return NULL;
	}

	FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
	file->abs_path = path;
	file->first_logical_cluster = target_file->first_logical_cluster;
	file->file_desc = 0;  // Default for now, can change, just put 0
	file->access = access;
	file->position = 0;
	file->file_size = target_file->file_size;

	int index = -1;
	for (int i = 0; i < MAX_DIRS; i++) {
		if (opened_dirs[i] == NULL) {
			index = i;
			break;
		}
	}

	// Checking if there's space in the array
	if (index != -1) {
		opened_dirs[index] = file;
		// printf("FileHandle added successfully at index %d\n", index);
		return file;
	}

	// adding failed
	free(file);
	return NULL;
}

int f_close(FileHandle* file) {
	if (file == NULL) {
		printf("f_close: NULL file.\n");
        return EOF;
	}
	int closed = 0;

	// remove from array of opened dir
	for (int i = 0; i < MAX_DIRS; i++) {
		if (opened_dirs[i] != NULL) {
			if (strcmp(opened_dirs[i]->abs_path, file->abs_path) == 0) {
				opened_dirs[i] = NULL;
				closed = 1;
			} 
		}
	}

	if (closed == 0) {
		printf("f_close: File not found.\n");
        return EOF;
	}

	printf("file found. freeing file...\n");
	free(file);
	return 0;
}

int f_seek(FileHandle* file, long offset, int whence) {
    if (file == NULL) {
        printf("ERROR: File is invalid.\n");
        return -1;
    }

    long new_pos;

    if (whence == SEEK_SET) { // Seeks from beginning of file
        new_pos = offset;
    } else if (whence == SEEK_CUR) { // Seeks from curr position of file
        new_pos = file->position + offset;
    } else if (whence == SEEK_END) { // Seeks from end of the file
        new_pos = file->file_size + offset;
    } else {
        printf("ERROR: Seek mode is invalid.\n");
        return -1;
    }

    // Checking if the new_pos is valid in the file
	// Should we also check if it fits within the file's size (aka max pos)?
    if (new_pos < 0) {
        printf("ERROR: Seek position is invalid.\n");
        return -1;
    }

    file->position = new_pos;

    return 0; // Returning 0 to indicate success~!
}

void f_rewind(FileHandle* file) {
    if (file == NULL) {
        printf("ERROR: No file is open.\n");
        return;
    }

    if (f_seek(file, 0, SEEK_SET) != 0) {
        printf("ERROR: Failed to f_rewind() file pointer.\n");
        return;
    }

    // printf("File pointer rewound to the start of the file.\n"); // Success message~!
}

// Open a directory 
DirectoryEntry* f_opendir(char* directory) {
	if (compare_filename(directory, &root_dir_entry)){
		printf("Directory opened: %s\n", directory);
		return &root_dir_entry;
	}

	char temp[strlen(directory) + 1];
    strcpy(temp, directory);
    
    char *token = strtok(temp, "/");
    
    // first token is "/", skip
    if (strcmp(token, "") == 0) {
        token = strtok(NULL, "/");
    }

    // DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	// sub_dir = &root_dir_entry;
	DirectoryEntry* sub_dir = &root_dir_entry;
	Directory* current_dir = root_dir; // traverse from root

    while (token != NULL) {
        printf("Looking up directory: %s...\n", token);
		current_dir = f_readdir(sub_dir);
		int found_dir = 0;

		if (current_dir == NULL){
			printf("f_opendir: cannot find given dir, %s. \n", token);
			return NULL;
		}

		for (int i = 0; i < num_dir_per_block; i++) {
			sub_dir = &current_dir->entries[i];
			
			// check if the filename matches
			if (compare_filename(token, sub_dir)) {
				if (sub_dir->type == 1){
					printf("Found: %s\n", token);
					found_dir = 1;
					break;
				}
			}
		}

		if (found_dir == 0){
			printf("f_opendir: cannot find given dir, %s. \n", token);
			return NULL;
		}

        token = strtok(NULL, "/");
    }

	printf("Successfully opened directory ---\n");

	return sub_dir;
}

int f_closedir(DirectoryEntry* dir_entry) {
	// check if given entry is null
	if (dir_entry == NULL) {
		printf("closedir: given NULL. \n");
        return -1;
    }

	// check if given entry is directory
	if(dir_entry->type != 1){
		printf("closedir: invalid directory entry. \n");
        return -1;
	}

	printf("closing entry: %s\n", dir_entry->filename);
	// no need to free, sub_entry did not use malloc
    return 0;
}

// returns ALL dir entries (array) under the given try
Directory* f_readdir(DirectoryEntry* entry) {
	// error check
	if (entry == NULL) {
		printf("readdir: given NULL entry.\n");
		return NULL;
	}

	if (entry->type != 1) {
		printf("readdir: given entry is not a directory.\n");
		return NULL;
	}

	if (entry->first_logical_cluster == EMPTY) {
		printf("readdir: given entry is empty.\n");
		return NULL;
	}

	// array of all dir entry
	Directory* dir = (Directory *) data_section[entry->first_logical_cluster].buffer;

	// check if empty directory
	if (strcmp(dir->entries[0].filename, "") == 0){
		printf("readdir: empty directory entries. Returning NULL.\n");
		return NULL;
	}

	printf("Reading directory entries for '%s'...\n", entry->filename);
	int i = 0;
	while (strcmp(dir->entries[i].filename, "") != 0) {
		printf("Entry %d %s\n", i, dir->entries[i].filename);
		if (i >= num_dir_per_block) {
			break;
		}
		i++;
	}

	return dir; 
}

// helper func to read 1 block of data
char* read_block(int cluster) {
	if (cluster < 0 || cluster >= EMPTY) {
		printf("ERROR: Block number is out of bounds.\n");
		return NULL;
	}

	char* buffer = (char*)malloc(BLOCK_SIZE);
	memcpy(buffer, data_section[cluster].buffer, BLOCK_SIZE);
	
	return buffer;
}

// read the contents of a file
int f_read(FileHandle *file, void* buffer, int bytes) {
	if (buffer == NULL) {
        printf("Error: Buffer pointer is NULL.\n");
        return -1; // Return error code
    }

	if (file == NULL) {
		printf("Error: File handle is NULL.\n");
		return -1; // Return error code
	}

	if (bytes < 0) {
		printf("Error: Number of bytes to read is negative.\n");
		return -1; // Return error code
	}

	int first_logical_cluster = file->first_logical_cluster;
	int file_size = file->file_size;

	if (file->position >= file_size) {
		printf("Error: File position is at or beyond the end of the file.\n");
		return 0; // Return 0 bytes read
	}

	if (strcmp(file->access, "r") != 0 && strcmp(file->access, "r+") != 0) {
		printf("Error: File is not open for reading.\n");
		return -1; // Return error code
	}

	// Check if the number of bytes to read exceeds the file size
	if (bytes > file_size) {
        bytes = file_size; 
    }

	// Traverse through the FAT to read the file
	int current_cluster = first_logical_cluster;
	int bytes_read = 0;
	int bytes_remaining = bytes;

	int i = 0;
	printf("Traversing FAT to read file contents...\n");
	while (bytes_remaining > 0) {
		// Read the current cluster
		char* data = read_block(current_cluster);
		int data_size = strlen(data);
		printf("Data block %d: %s - (size: %d)\n", i, data, data_size);

		// Calculate the number of bytes to read from the current cluster
		int bytes_to_read = (bytes_remaining < data_size) ? bytes_remaining : data_size;

		// Copy the data into the buffer
		memcpy(buffer + bytes_read, data, bytes_to_read);
		free(data);

		// Update the number of bytes read and remaining
		bytes_read += bytes_to_read;
		bytes_remaining -= bytes_to_read;

		// Move to the next cluster
		current_cluster = FAT[current_cluster].block_number;

		if (current_cluster == END_OF_FILE) {
			break;
		}

		i++;
	}

	// Add null terminator to the buffer
	((char*)buffer)[bytes] = '\0';

	// Update the file position
	// file->position += bytes_read;

	printf("Successfully read %d bytes from the file.\n", bytes_read);

	return bytes_read;
}

/* int f_write(FileHandle *file, void *buffer, size_t bytes) {
    if (file == NULL || buffer == NULL) {
        return -1; // Invalid arguments
    }

    // Get the start cluster of the file
    FATEntry *start_cluster;
	start_cluster->block_number = file->first_logical_cluster;

    // Calculate the current file size
    uint32_t current_file_size = file->file_size;

    // Calculate the new file size after writing
    uint32_t new_file_size = current_file_size + bytes;

    // Calculate the number of clusters needed to store the new data
    uint32_t new_clusters_needed = (new_file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Allocate additional clusters if necessary
    if (new_clusters_needed > file->file_size/BLOCK_SIZE) {
        fat_allocate_cluster_chain(start_cluster, file->first_logical_cluster, new_file_size);
    }

    // Write the data into the file clusters
    uint8_t *data_ptr = (uint8_t *)buffer;
    uint32_t remaining_bytes = bytes;
    uint32_t bytes_written = 0;

    while (remaining_bytes > 0) {
        // Calculate the offset within the current cluster
        uint32_t cluster_offset = current_file_size % BLOCK_SIZE;

        // Calculate the number of bytes to write in this iteration
        uint32_t bytes_to_write = remaining_bytes;
        if (bytes_to_write > (BLOCK_SIZE - cluster_offset)) {
            bytes_to_write = BLOCK_SIZE - cluster_offset;
        }

        // Write data to the file using fat_read_file_contents
        fat_read_file_contents(start_cluster, current_file_size + bytes_written, data_ptr);

        // Update pointers and counters
        data_ptr += bytes_to_write;
        bytes_written += bytes_to_write;
        remaining_bytes -= bytes_to_write;
    }

	const char *path = file->abs_path;
	char *extracted_filename = extract_filename(path);

    // Update the file size in the directory entry
    fat_update_directory_entry(extracted_filename, start_cluster, new_file_size);

	free(extracted_filename);
    // Return the number of bytes written
    return bytes_written;
} */

// int f_remove(const char* path) {
// 	return 0;
// }

// int f_stat(FileHandle *file, struct stat *buffer) {
// 	return 0;
// }

// int f_mkdir(char* path) {
// 	return 0;
// }

// int f_rmdir(char* path) {
// 	return 0;
// }

// to be called before the mainloop or testing
void fs_mount(char *diskname) {
	printf("==== MOUNTING... ====\n");
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
	int iter = 10;
	if ((int)sb.file_size_blocks < 10){
		iter = (int)sb.file_size_blocks;
	}
	for (int i = 0; i < iter; i++) {
	    //replace 10 with sb.file_size_blocks to print entire data section
        printf("FAT cell %d: %d\n", i, FAT[i].block_number); 
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
	printf("First block: %d\n", root_dir_entry.first_logical_cluster);
    // printf("file size: %d\n", root_dir_entry.file_size);
    printf("File type: %d\n", root_dir_entry.type);
	printf("\n");
	
	//read + define datablock section
    data_section = (datablock *)malloc(sb.file_size_blocks * sizeof(datablock));
	fseek(disk, BLOCK_SIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
	fread(data_section, BLOCK_SIZE, sb.file_size_blocks, disk); //Directory is size: 512 bytes
	printf("Data section in use (max first 10 blocks): \n");
	printf("If directory, print the first entry\n");

    for (int i = 0; i < iter; i++) {
        // replace 10 with sb.file_size_blocks to print entire data section
		// print block content only if in use
		if (bitmap.bitmap[i] == 0){
			printf("Block %d: %s\n", i, data_section[i].buffer);
		}
    }
    printf("\n");

    // Access the first block in the data section to verify the copy
    root_dir = (Directory *) data_section[0].buffer;
    printf("Root dir's first entry filename: %s\n", root_dir->entries[0].filename);
    printf("Root dir's first entry first block: %d\n", root_dir->entries[0].first_logical_cluster);
    printf("Root dir's first entry type: %d\n", root_dir->entries[0].type);
	
	fclose(disk);
	printf("Mounting done\n \n");
}

void fs_unmount(char *diskname) {
	printf("==== UNMOUNTING... ====\n");
	// extern superblock, FAT, bitmap, rootdir, data_section globals declared in header, defined in this func
	FILE *disk = fopen(diskname, "wb");
	if (disk == NULL) {
	    fprintf(stderr, "Error: Failed to open disk file '%s'\n", diskname);
	    exit(EXIT_FAILURE);
	}
	
	//read + define superblock
	fseek(disk, 0, SEEK_SET);
	fwrite(&sb, sizeof(superblock), 1, disk);
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
	fwrite(FAT, (sb.file_size_blocks * sizeof(FATEntry)), 1, disk);
	printf("FAT Info:\n");
	int iter = 10;
	if ((int)sb.file_size_blocks < 10){
		iter = (int)sb.file_size_blocks;
	}
	for (int i = 0; i < iter; i++) {
	    //replace 10 with sb.file_size_blocks to print entire data section
        printf("FAT cell %d: %d\n", i, FAT[i].block_number); 
	}
	printf("\n");
	
	//read + define bitmap
	fseek(disk, BLOCK_SIZE * sb.FREEMAP_offset, SEEK_SET);
	fwrite(&bitmap, sizeof(BitmapBlock), 1, disk);
	printf("Bitmap Info:\n");
	for (int i = 0; i < BLOCK_SIZE; i++) {
	    printf("%d", (bitmap.bitmap[i]));
	}
	printf("\n");
	
	//read + define rootdirentry
	fseek(disk, BLOCK_SIZE * sb.ROOTDIR_offset, SEEK_SET);
	fwrite(&root_dir_entry, sizeof(DirectoryEntry), 1, disk); //Directory Entry is size: 19 bytes
	printf("Root directory: %s\n", root_dir_entry.filename);
	printf("First block: %d\n", root_dir_entry.first_logical_cluster);
    // printf("file size: %d\n", root_dir_entry.file_size);
    printf("File type: %d\n", root_dir_entry.type);
	printf("\n");
	
	//read + define datablock section
	fseek(disk, BLOCK_SIZE * sb.DATA_offset, SEEK_SET); // Move to first block in data section
	fwrite(data_section, BLOCK_SIZE, sb.file_size_blocks, disk); //Directory is size: 512 bytes
	printf("Data section in use (max first 10 blocks): \n");
	printf("If directory, print the first entry\n");

    for (int i = 0; i < iter; i++) {
        // replace 10 with sb.file_size_blocks to print entire data section
		// print block content only if in use
		if (bitmap.bitmap[i] == 0){
			printf("Block %d: %s\n", i, data_section[i].buffer);
		}
    }
    printf("\n");

	fclose(disk);
	printf("Unmounting done\n \n");
}

void is_success(int val) {
	if (val == 0) {
		printf("Successfully closed file~!\n");
		return;
	}

	printf("Failure, cannot close file~~\n");
}

void test_opendir_readdir_disk_1() {
	printf("\n\n==== TESTING F_OPENDIR() ====\n");

	printf("1. Open root directory\n");
	DirectoryEntry* opened_root = f_opendir("/");
	if (opened_root != NULL){
		print_subdir(opened_root);
	}

	printf("\n2. Open /Desktop directory\n");
	DirectoryEntry* opened_desktop = f_opendir("/Desktop");
	if (opened_desktop != NULL){
		print_subdir(opened_desktop);
	}

	printf("\n3. Open /Download directory\n");
	DirectoryEntry* opened_download = f_opendir("/Download");
	if (opened_download != NULL){
		print_subdir(opened_download);
	}

	printf("\n4. Attempt to open /Hello.txt (not a directory)\n");	
	DirectoryEntry* opened_hello = f_opendir("Hello.txt");
	if (opened_hello == NULL){
		printf("Cannot open Hello.txt because it is not a directory.\n");
	}

	printf("\n5. Open /Desktop/CS355 directory\n");
	DirectoryEntry* opened_cs355 = f_opendir("/Desktop/CS355");
	if (opened_cs355 != NULL) {
		print_subdir(opened_cs355);
	}

	printf("\n6. Open /Desktop/CS355/labs/lab1 \n");
	DirectoryEntry* opened_lab1 = f_opendir("/Desktop/CS355/labs/lab1");
	if (opened_lab1 != NULL){
		print_subdir(opened_lab1);
	}

	printf("\n7. Attempt to open /Desktop/CS355/labs/lab1/essay (non-existent)\n");
	DirectoryEntry* opened_essay = f_opendir("/Desktop/CS355/labs/lab1/essay");
	if (opened_essay == NULL){
		printf("Cannot open essay because it does not exist.\n");
		printf("Test passed.");
	}

	printf("\n\n==== TESTING F_READDIR() ====\n"); // print statement in f_readdir()
	printf("1. Read root directory\n");
	Directory* sub_entries = f_readdir(opened_root);
	print_dir(sub_entries);

	printf("\n2. Read /Desktop directory\n");
	sub_entries = f_readdir(opened_desktop);

	printf("\n3. Read /Download directory\n"); 
	sub_entries = f_readdir(opened_download);

	printf("\n4. Read /CS355 directory\n");
	sub_entries = f_readdir(opened_cs355);

	printf("\n5. Read /labs/lab1 directory\n");
	sub_entries = f_readdir(opened_lab1);

	printf("\n6. Read /labs/lab1/essay directory (non-existent)\n");
	sub_entries = f_readdir(opened_essay);

	printf("\n7. Read /Hello.txt (not a directory)\n");
	sub_entries = f_readdir(opened_hello);

	printf("\n\n==== TESTING F_CLOSEDIR() ====\n"); 
	printf("1. Close root directory\n");
	int ret = f_closedir(opened_root);
	if (ret == 0){
		printf("close success~\n");
	} else {
		printf("close failed!!!\n");
		printf("Not expected :(\n");
	}

	printf("\n2. Close /Desktop directory\n");
	ret = f_closedir(opened_desktop);
	if (ret == 0){
		printf("close success~\n");
	} else {
		printf("close failed!!!\n");
		printf("Not expected :(\n");
	}

	printf("\n3. Close /Download directory\n"); 
	ret = f_closedir(opened_download);
	if (ret == 0){
		printf("close success~\n");
	} else {
		printf("close failed!!!\n");
		printf("Not expected :(\n");
	}

	printf("\n4. Close /CS355 directory\n");
	ret = f_closedir(opened_cs355);
	if (ret == 0){
		printf("close success~\n");
	} else {
		printf("close failed!!!\n");
		printf("Not expected :(\n");
	}

	printf("\n5. Close /labs/lab1 directory\n");
	ret = f_closedir(opened_lab1);
	if (ret == 0){
		printf("close success~\n");
	} else {
		printf("close failed!!!\n");
		printf("Not expected :(\n");
	}

	printf("\n6. Close /labs/lab1/essay directory (non-existent)\n");
	ret = f_closedir(opened_essay);
	if (ret == 0){
		printf("close success~\n");
		printf("Not expected :(\n");
	} else {
		printf("close failed!!!\n");
		printf("Fail expected -- Null entry\n");
	}

	printf("\n7. Close /Hello.txt (not a directory)\n");
	ret = f_closedir(opened_hello);
	if (ret == 0){
		printf("close success~\n");
		printf("Not expected :(\n");
	} else {
		printf("close failed!!!\n");
		printf("Fail expected -- Null entry \n");
	}
}

void test_hardcoded_fread_disk_1() {
	printf("\n\n==== TESTING F_READ() ====\n");

	printf("Read /Desktop/blog_1.txt\n");
	printf("1. Open /Desktop/Blog1.txt\n");
	printf("GRACEEE~~\n");

	// FEED IN THE FILE HANDLE FROM F_OPEN
	printf("\n2. Read 56 bytes (file size)\n");
	char buffer[512];
	int bytes = f_read(NULL, buffer, 56);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n3. Read 6 bytes only\n");
	bytes = f_read(NULL, buffer, 6);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n4. Attempt to read 100 bytes (file size exceeded)\n");
	bytes = f_read(NULL, buffer, 100);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);
}

void test_open_read_disk_1() {
	printf("\n\n==== TESTING F_OPEN() ====\n");

	printf("1. Open /Desktop/blog_1.txt\n");
	FileHandle* blog_1 = f_open("/Desktop/blog_1.txt", "r");
	print_file_handle(blog_1);

	printf("\n2. Attempt to open /Desktop/blog_2.txt (non-existent)\n");
	FileHandle* blog_2 = f_open("/Desktop/blog_2.txt", "r");
	print_file_handle(blog_2);

	printf("\n3. Attempt to open /Desktop/CS355 (folder)\n");
	FileHandle* cs355 = f_open("/Desktop/CS355", "r");
	print_file_handle(cs355);

	printf("\n4. Open /Desktop/CS355/hw1.txt\n");
	FileHandle* hw1 = f_open("/Desktop/CS355/hw1.txt", "r");
	print_file_handle(hw1);

	printf("\n5. Open /Hello.txt\n");
	FileHandle* hello = f_open("/Hello.txt", "r");
	print_file_handle(hello);

	printf("\n\n==== TESTING F_READ() ====\n");
	printf("1. Read /Desktop/blog_1.txt\n");
	
	printf("\nRead the whole file\n");
	char buffer[512];
	int bytes = f_read(blog_1, buffer, 56);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\nRead 6 bytes only\n");
	bytes = f_read(blog_1, buffer, 6);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\nAttempt to read 100 bytes (file size exceeded)\n");
	bytes = f_read(blog_1, buffer, 100);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n2. Read /Desktop/CS355/hw1.txt\n");
	printf("Read the whole file\n");
	bytes = f_read(hw1, buffer, 9);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\nRead 0 bytes\n");
	bytes = f_read(hw1, buffer, 0);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n3. Read /Desktop/blog_2.txt (non-existent)\n");
	bytes = f_read(blog_2, buffer, 10);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n4. Read /Desktop/CS355 (folder)\n");
	bytes = f_read(cs355, buffer, 10);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n5. Read /Hello.txt\n");
	bytes = f_read(hello, buffer, 10);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n6. Read from NULL buffer\n");
	bytes = f_read(blog_1, NULL, 10);
	printf("Bytes read: %d\n", bytes);

	// Tests for f_seek()
	printf("\n\n==== TESTING F_SEEK() ====\n");
	printf("1. Change File Position Location for /Desktop/blog_1.txt\n");
	printf("a. Check with SEEK_SET\n");
	printf("Expected Updated File Position: 1\n");

	int success = f_seek(blog_1, 1, SEEK_SET);
	if (success == 0) {
		printf("Successfully fseek()'ed\n");
		printf("Updated File Position: %d\n", blog_1->position);
	} else {
		printf("Failed to fseek()\n");
	}

	printf("\nb. Check with SEEK_CURR\n");
	printf("Expected Updated File Position: 2\n");
	success = f_seek(blog_1, 1, SEEK_CUR);
	if (success == 0) {
		printf("Successfully fseek()'ed\n");
		printf("Updated File Position: %d\n", blog_1->position);
	} else {
		printf("Failed to fseek()\n");
	}

	printf("\nc. Check with SEEK_END\n");
	printf("Expected Updated File Position: 56\n");
	success = f_seek(blog_1, 0, SEEK_END);
	if (success == 0) {
		printf("Successfully fseek()'ed\n");
		printf("Updated File Position: %d\n", blog_1->position);
	} else {
		printf("Failed to fseek()\n");
	}

	// Tests for f_rewind()
	printf("\n\n==== TESTING F_REWIND() ====\n");
	printf("1. Rewind File Position Location for /Desktop/blog_1.txt\n");
	printf("Expected Updated File Position: 0\n");

	f_rewind(blog_1);
	printf("Rewinded File Position: %d\n", blog_1->position);

	// tests for f_close()
	printf("\n\n==== TESTING F_CLOSE() ====\n");
	printf("1. Close /Desktop/blog_1.txt\n");
	int res = f_close(blog_1);
	if (res == 0){
		printf("close success~\n");
	} else {
		printf("close failed :(\n");
		printf("not expected!!\n");
	}

	printf("\n2. Attempt to close /Desktop/blog_2.txt (non-existent)\n");
	res = f_close(blog_2);
	if (res == 0){
		printf("close success~\n");
		printf("not expected!!\n");
	} else {
		printf("close failed :(\n");
		printf("Fail expected -- Null file\n");
	}

	printf("\n3. Attempt to close /Desktop/CS355 (folder)\n");
	res = f_close(cs355);
	if (res == 0){
		printf("close success~\n");
		printf("not expected!!\n");
	} else {
		printf("close failed :(\n");
		printf("Fail expected -- Null file\n");
	}

	printf("\n4. Close /Desktop/CS355/hw1.txt\n");
	res = f_close(hw1);
	if (res == 0){
		printf("close success~\n");
	} else {
		printf("close failed :(\n");
		printf("not expected!!\n");
	}

	printf("\n5. Close /Hello.txt\n");
	res = f_close(hello);
	if (res == 0){
		printf("close success~\n");
	} else {
		printf("close failed :(\n");
		printf("not expected!!\n");
	}
}

void test_disk_1() {
	fs_mount("./disks/fake_disk_1.img");
	test_opendir_readdir_disk_1();
	// test_hardcoded_fread_disk_1();
	test_open_read_disk_1();

	// free from fs_mount
	free(FAT);
	free(data_section);
}

int main(void) {
	// Runs mount and tests for fake_disk_1
	test_disk_1();

}
