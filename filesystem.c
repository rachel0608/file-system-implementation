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

// Seek offset constants
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define MAX_DIRS 100
#define EMPTY 65535 // first_logical_cluster value for empty directory

FileHandle* opened_dirs[MAX_DIRS] = { NULL }; // All files that are open
superblock sb;
FATEntry *FAT;
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
Directory *root_dir;
datablock *data_section; // 1MB = 2048 blocks * 512 bytes = 1048576 bytes
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
		printf("First logical cluster: %d\n", sub_dir->first_logical_cluster);
		// printf("File size: %d\n\n", sub_dir->file_size);
	}
}

// print all dir entries in a directory
void print_dir(Directory* entry) {
	for(int i = 0; i < num_dir_per_block; i++){
		print_subdir(&entry->entries[i]);
	}
}

// print a subdirectory
void print_filehandle(FileHandle* file) {
	if (file != NULL) {
		printf("FileHandle Info:\n");
		printf("Absolute Path: %s\n", file->abs_path);
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

	printf("Path: %s\n", path);
	printf("Reformatted Path: %s\n", new_path);

	DirectoryEntry* open_entry = f_opendir(new_path);
	Directory* all_dirs = f_readdir(open_entry);

	for (int i = 0; i < num_dir_per_block; i++) {
		// check if the filename matches
		DirectoryEntry* dir = &all_dirs->entries[i];
		
		if ((strcmp(dir->filename, "") != 0) && (strcmp(open_entry->filename, dir->filename) == 0)) {
			printf("Directory opened: %s\n", open_entry->filename);

			for (int j = 0; j < MAX_DIRS; j++) {
				printf("YAYEET    %s\n", opened_dirs[j]->abs_path);
				if ((opened_dirs[i] != NULL) && (strcmp(opened_dirs[j]->abs_path, path) == 0)) {
					printf("HELLLOO ARGH\n");
					return opened_dirs[i];
				}
			}
		}
	}

	FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
	file->abs_path = path;
	file->file_desc = 0;  // Default for now, can change, just put 0
	file->access = access;
	file->position = 0;

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

	return NULL;
}

int f_close(FileHandle* file) {
	if (file == NULL) {
		perror("ERROR: File does not exist.\n");
        return EOF;
	}

	int i = 0;
	int closed = 0;

	while (opened_dirs[i]) {
		if (strcmp(opened_dirs[i]->abs_path, file->abs_path) == 0) {
			opened_dirs[i] = NULL;
			closed = 1;
		}

		i++;
	}

	if (closed == 0) {
		perror("ERROR: File does not exist.\n");
        return EOF;
	}

	free(file);
	return 0;
}

// more cleanup?
	// TODO: Remove dir_entry's FileHandle from the global open files array
	// Concern: How do we compare DirectoryEntry and FileHandle?????
	// FileHandle* open_dirs[MAX_DIRS];

// int f_write(FileHandle file, void* buffer, size_t bytes);

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

// int f_stat(FileHandle file, struct stat *buffer);
// int f_remove(const char* filename);


// Open a directory 
// Currently open in root directory
// Might fix to return a DIR struct instead of DirectoryEntry
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

    DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	sub_dir = &root_dir_entry;

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
	if (dir_entry == NULL) {
        return -1;
    }

    free(dir_entry);

    return 0;
}

// need more testing
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
	Directory* dir = (Directory*)malloc(sizeof(Directory));
	dir = (Directory *) data_section[entry->first_logical_cluster].buffer;

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

// read the contents of a file
int f_read(FileHandle *file, void* buffer, int bytes) {
	int first_logical_cluster = 3; // file->first_logical_cluster when open() works properly
	int file_size = 15; // file->file_size when open() works properly
	int result = 0;

	if (buffer == NULL) {
        printf("Error: Buffer pointer is NULL.\n");
        return -1; // Return error code
    }

	// UNCOMMENT THIS WHEN OPEN() WORKS
	// if (file == NULL) {
	// 	printf("Error: File handle is NULL.\n");
	// 	return -1; // Return error code
	// }

	if (bytes < 0) {
		printf("Error: Number of bytes to read is negative.\n");
		return -1; // Return error code
	}

	// UNCOMMENT THESE WHEN OPEN() WORKS
	// if (file->position >= file_size) {
	// 	printf("Error: File position is at or beyond the end of the file.\n");
	// 	return 0; // Return 0 bytes read
	// }

	// if (file->access != "r") {
	// 	printf("Error: File is not open for reading.\n");
	// 	return -1; // Return error code
	// }

	// Check if the number of bytes to read exceeds the file size
	if (bytes > file_size) {
        bytes = file_size; 
    }

	for (int i = 0; i < bytes; i++) {
		((char*)buffer)[i] = data_section[first_logical_cluster].buffer[i];
		result++;
	}

	((char*)buffer)[bytes] = '\0';

	return result;
}

// to be called before the mainloop
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
	for (int i = 0; i < 10; i++) {
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
	printf("Data section (only 10 blocks shown): \n");
	printf("If directory, print the first entry\n");
    for (int i = 0; i < 10; i++) {
        //replace 10 with sb.file_size_blocks to print entire data section
        printf("Block %d: %s\n", i, data_section[i].buffer);
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

void print_file_handle(FileHandle* fh) {
	if (fh == NULL) {
		printf("ERROR: Cannot print file handle because file handle is NULL.\n");
		return;
	}

	printf("Printing FILE HANDLE==============\n");
	printf("HERE10\n");
	printf("abs_path:  %s\n", fh->abs_path);
	printf("HERE11\n");
	printf("file_desc:  %d\n", fh->file_desc);
	printf("HERE12\n");
	printf("access:  %s\n", fh->access);
	printf("HERE13\n");
	printf("position:  %d\n", fh->position);
	printf("HERE14\n");
	printf("file_size:  %d\n", fh->file_size);
}

void test_opendir_readdir_disk_1() {
	printf("==== TESTING F_OPENDIR() ====\n");

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
	}

	printf("\n==== TESTING F_READDIR() ====\n"); // print statement in f_readdir()
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

}

void test_read_disk_1() {
	printf("\n==== TESTING F_READ() ====\n");

	printf("Read /Desktop/Blog1.txt\n");
	printf("1. Open /Desktop/Blog1.txt\n");
	printf("GRACEEE~~\n");

	// FEED IN THE FILE HANDLE FROM F_OPEN
	printf("\n2. Read 15 bytes (file size)\n");
	char buffer[16];
	int bytes = f_read(NULL, buffer, 16);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n3. Read 6 bytes only\n");
	bytes = f_read(NULL, buffer, 6);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);

	printf("\n4. Attempt to read 1000 bytes (file size exceeded)\n");
	bytes = f_read(NULL, buffer, 1000);
	printf("Bytes read: %d\n", bytes);
	printf("Buffer: %s\n", buffer);
	
}

void test_disk_1() {
	fs_mount("./disks/fake_disk_1.img");
	test_opendir_readdir_disk_1();
	test_read_disk_1();
}


int main(void) {
	// Mount fake_disk_2.img
	// fs_mount("./disks/fake_disk_2.img");

	// Mount fake_disk.img
	test_disk_1();

	// *****
	// Testing for f_open():

	// printf("\n=== testing f_open on Hello.txt ===\n");
	// // char buffer[20];
	// FileHandle* file = f_open("/Hello.txt", "r");
	// print_filehandle(file);

	// FileHandle* file2 = f_open("/Hello.txt", "r");
	// print_filehandle(file2);

	// printf("\n=== testing f_open on blog_1.txt ===\n");
	// // char buffer[20];
	// file = f_open("/Desktop/blog_1.txt", "r");
	// print_filehandle(file);

	// printf("\n=== testing f_open on hw1.txt ===\n");
	// // char buffer[20];
	// file = f_open("/Desktop/CS355", "r");
	// print_filehandle(file);

	// *****
	// Testing for f_read():
	
	// printf("\n=== testing f_read on Hello.txt ===\n");
	// char buffer[20];
	// printf("Attempting to read 15 bytes (file size)\n");
	// int bytes = f_read(NULL, buffer, 20);
	// printf("bytes read: %d\n", bytes);
	// printf("buffer: %s\n", buffer);

	// printf("Attempting to read 6 bytes only\n");
	// bytes = f_read(NULL, buffer, 6);
	// printf("bytes read: %d\n", bytes);
	// printf("buffer: %s\n", buffer);

	// printf("Attempting to read 1000 bytes (file size exceeded)\n");
	// bytes = f_read(NULL, buffer, 1000);
	// printf("bytes read: %d\n", bytes);
	// printf("buffer: %s\n", buffer);

	// ----------------------------------------

	// Mount fake_disk_4_folders.img
	// fs_mount("./disks/fake_disk_4_folders.img");

	// printf("=== test: open root dir ===\n");
	// DirectoryEntry* opened_entry = f_opendir("/");
	// printf("opened directory: \n");
	// print_subdir(opened_entry);
	// printf("opendir(/) done\n \n");

	// printf("=== test: open folder2 dir ===\n");
	// opened_entry = f_opendir("folder2");
	// if (opened_entry != NULL){
	// 	printf("opened directory: \n");
	// 	print_subdir(opened_entry);
	// 	printf("opendir(folder2) done\n \n");
	// }

	// printf("=== test: open folder1 dir ===\n");
	// opened_entry = f_opendir("folder1");
	// printf("opened directory: \n");
	// print_subdir(opened_entry);
	// printf("opendir(folder1) done\n \n");

	// printf("=== test: open file1.txt ===\n");
	// opened_entry = f_opendir("file1.txt");
	// if (opened_entry != NULL){
	// 	printf("opened directory: \n");
	// 	print_subdir(opened_entry);
	// 	printf("opendir(file1) done\n \n");
	// }

	// printf("===testing readdir on folder1 ===\n");
	// Directory* sub_entries = f_readdir(opened_entry);
	// print_dir(sub_entries);

}
