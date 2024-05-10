// filesystem.c
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

FileHandle* open_dirs[MAX_DIRS]; // All files that are open
superblock sb;
FATEntry *FAT;
BitmapBlock bitmap;
DirectoryEntry root_dir_entry;
Directory *root_dir;
datablock *data_section; // 1MB = 2048 blocks * 512 bytes = 1048576 bytes
int num_dir_per_block = BLOCK_SIZE / sizeof(DirectoryEntry); // 26 entries per block

// helper function to compare filename
bool compare_filename(const char* filename, const DirectoryEntry* dir_entry) {
	// printf("comparing dir/file names:\n");
	// printf("given directory name: %s\n", filename);
	// printf("subdir found: %s\n", dir_entry->filename);

	//printf("ext: %s\n", dir_entry->ext);
	// printf("type: %s\n", dir_entry->type);
	// printf("first cluster: %s\n", dir_entry->first_logical_cluster);
    // if the entry is a directory
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
	if(strcmp(sub_dir->filename, "") != 0){
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

	printf("HERE4\n");

	// check user permission -- if user can R, then cannot W
	// if user cannot R or W, then just return null
	// For now not done since we don't have user type id

	// parse path first -- path included file names. (/home/a.txt --> /home)
	char new_path[2048];
	strncpy(new_path, path, sizeof(new_path) - 1);
	reformat_path(new_path);

	printf("\nHERE5\n");
	printf("Path: %s\n", path);
	printf("Reformatted Path: %s\n", new_path);

	DirectoryEntry* open_entry = f_opendir(new_path);

	if (open_entry == NULL) {
		printf("ERROR: File at this path not found - %s\n", new_path);
		return NULL; 
	}

	// check if this file exists in the give path
	// if yes, just return file handle
	// if not, then create a new file handle and return
	// look into FAT[0], find a DirectoryEntry with the same filename
	int bytes_count = 0;
	DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	
	while (bytes_count < BLOCK_SIZE) {
		// retrieve the first 20 bytes of FAT[0]
		memcpy(sub_dir, root_dir + bytes_count, sizeof(DirectoryEntry));
		print_subdir(sub_dir);

		// check if the filename matches
		if (strcmp(open_entry->filename, sub_dir->filename) == 0) {
			printf("Directory opened: %s\n", open_entry->filename);

			int i = 0;

			while (open_dirs[i]) {
				if (strcmp(open_dirs[i]->abs_path, path) == 0) {
					open_dirs[i] = NULL;
					i++;
				}
			}

			return open_dirs[i];
		} else {
			// move to the next DirectoryEntry
			bytes_count += sizeof(DirectoryEntry);
			printf("bytes_count: %d\n", bytes_count);
		}
	}

	free(sub_dir);

	FileHandle* file = (FileHandle*)malloc(sizeof(FileHandle));
	file->abs_path = path;
	file->file_desc = 0;  // Default for now, can change, just put 0
	file->access = access;
	file->position = 0;

	int j = 0;

	while (open_dirs[j] != NULL) {
		j++;
	}

	open_dirs[j] = file;
	return file;
}

int f_close(FileHandle* file) {
	if (file == NULL) {
		perror("ERROR: File does not exist.\n");
        return EOF;
	}

	int i = 0;
	int closed = 0;

	while (open_dirs[i]) {
		if (strcmp(open_dirs[i]->abs_path, file->abs_path) == 0) {
			open_dirs[i] = NULL;
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
	for (int i = 0; i < num_dir_per_block; i++) {
		DirectoryEntry* sub_dir = &root_dir->entries[i];
		// print_subdir(sub_dir);

		// check if the filename matches
		if (compare_filename(directory, sub_dir)) {
			if (sub_dir->type == 1){
				printf("Directory opened: %s\n", directory);
				return sub_dir;
			} else {
				printf("f_opendir: %s is not a directory \n", directory);
				return NULL;
			}
			
		} 
	}

	// // look into FAT[0], find a DirectoryEntry with the same filename
	// int bytes_count = 0;
	// DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));

	// while (bytes_count < BLOCK_SIZE) {
	// 	// retrieve the first 20 bytes of FAT[0]
	// 	memcpy(sub_dir, root_dir + bytes_count, sizeof(DirectoryEntry));
	// 	//print_subdir(sub_dir);

	// 	// check if the filename matches
	// 	if (compare_filename(directory, sub_dir)) {
	// 		if (sub_dir->type == 1){
	// 			printf("Directory opened: %s\n", directory);
	// 			return sub_dir;
	// 		} else {
	// 			printf("f_opendir: %s is not a directory \n", directory);
	// 			return NULL;
	// 		}
	// 	} else {
	// 		// move to the next DirectoryEntry
	// 		bytes_count += sizeof(DirectoryEntry);
	// 	}
	// }
	printf("f_opender: given directory not found. \n");
	return NULL;
}

int f_closedir(DirectoryEntry* dir_entry) {
	if (dir_entry == NULL) {
        return -1;
    }

    free(dir_entry);

    return 0;
}

// need to debug a bit - Cecilia
Directory* f_readdir(DirectoryEntry* entry) {
	// look into FAT[0], find a DirectoryEntry with the same filename
	// int bytes_count = 0;

	// array of all dir entry
	Directory* dir = (Directory*)malloc(sizeof(Directory));
	dir = (Directory *) data_section[entry->first_logical_cluster].buffer;
	
	// DirectoryEntry* sub_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
	// int i = 0;

	// while (bytes_count < BLOCK_SIZE) {
	// 	// retrieve the first 20 bytes of FAT[0]
	// 	memcpy(sub_dir, entry + bytes_count, sizeof(DirectoryEntry));
	// 	print_subdir(sub_dir);

	// 	// check if current entry is valid
	// 	if (strcmp(sub_dir->filename, "") == 0) {
	// 		printf("End of dir entry\n");
	// 		break;
	// 	}

	// 	sub_dir_arr->entries[i] = *sub_dir;

	// 	// move to the next DirectoryEntry
	// 	bytes_count += sizeof(DirectoryEntry);
	// 	// printf("bytes_count: %d\n", bytes_count);
	// 	i++;
	// }

	return dir; 
}


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
    // printf("file size: %d\n", root_dir_entry.file_size);
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
    // printf("Root dir's first entry file size: %d\n", root_dir->entries[0].file_size);
    printf("Root dir's first entry first block: %d\n", root_dir->entries[0].first_logical_cluster);
    printf("Root dir's first entry type: %d\n", root_dir->entries[0].type);
	
	fclose(disk);
	printf("mounting done\n \n");
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

int main(void) {
	// Mount the filesystem
	fs_mount("fake_disk_3_folders.img");
	// fs_mount("fake_disk.img");

	printf("=== test: open root dir ===\n");
	DirectoryEntry* opened_entry = f_opendir("/");
	printf("opened directory: \n");
	print_subdir(opened_entry);
	printf("opendir(/) done\n \n");

	// printf("=== test: open folder1 dir ===\n");
	// opened_entry = f_opendir("folder1");
	// printf("opened directory: \n");
	// print_subdir(opened_entry);
	// printf("opendir(folder1) done\n \n");

	// printf("=== test: open folder2 dir ===\n");
	// opened_entry = f_opendir("folder2");
	// if (opened_entry != NULL){
	// 	printf("opened directory: \n");
	// 	print_subdir(opened_entry);
	// 	printf("opendir(folder2) done\n \n");
	// }

	// printf("=== test: open file1.txt ===\n");
	// opened_entry = f_opendir("file1.txt");
	// if (opened_entry != NULL){
	// 	printf("opened directory: \n");
	// 	print_subdir(opened_entry);
	// 	printf("opendir(file1) done\n \n");
	// }

	printf("===testing readdir on root ===\n");
	Directory* sub_entries = f_readdir(opened_entry);
	print_dir(sub_entries);

	// FileHandle* fh = f_open("/file1.txt", "r");
	// printf("HERE2\n");
	// print_file_handle(fh);
	// printf("HERE3\n");
	
	// Open a file
}
