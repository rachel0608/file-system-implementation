// This is just pseudocode for places where it states == for
// string comparisons, they will be replaced by the
// appropriate/proper strcmp()
FileHandle* f_open(char* filename, char* access) {
  // if access == ‘r’ or ‘w’ or ‘a’ {
  //   // Checking if file exists
  //   DirectoryEntry* dir = find_file(filename);

  //   if file exists {
  //     Make FileHandle for/pointing to this file
  //     return FileHandle
  //   } else if file does not exist and access == ‘w’ or ‘a’ {
  //     // Create file with filename
  //     // Call fat_allocate_cluster_chain() allocate a chain of clusters for a new file
  //     Make FileHandle for/pointing to this file
  //     return FileHandle
  //   } else {
  //     print error;
  //     return NULL;
  //   }
  // }
  
  // print error;
  // return NULL;
}

int f_read(FileHandle file, void* buffer, size_t bytes) {
  // Read bytes from the current position of file (FileHandle file’s position value)
  // if error {
  //   print error
  //   return 0; (Unix man says return short item count or zero)
  // }

  // fat_read_file_contents(FatEntry *start_cluster, uint32_t file_size, uint8_t buffer);
  // return bytes
}

int f_write(FileHandle file, void* buffer, size_t bytes) {
  // if bytes == 0 {
  //   return 0; (Unix man says to)
  // }

  // // Write bytes from buffer to current position of file
  // if error {
  //   Return & print error
  // }

  // fat_allocate_cluster_chain(FatEntry* start_cluster, uint32_t file_size);
  // fat_update_directory_entry(const char* filename, FatEntry *start_cluster, uint32_t file_size);

  // return bytes
}

void f_close(FileHandle file) {
  // Close the file by handling its FileHandle
}

int f_seek(FileHandle file, long offset, int whence) {
  // Go to a new position in the file
  // Set file’s position (based on offset and whence)

  // Maybe consider whence parameter value == “SEEK_SET”, “SEEK_CUR”, “SEEK_END” like unix
}

void f_rewind(FileHandle file) {
  // Go to beginning position in the file
  // Set file’s position to beginning of file
}

int f_stat(FileHandle file, struct stat *buffer) { // returns int - success or not
  // f_stat() gets file info and puts it into stat
  // Get file info and put into buffer

  // if error {
  //   return error and -1
  // }

  // return 0 for success
}

int f_remove(const char* filename) {
  // fat_free_cluster_chain(FatEntry *start_cluster);
  // fat_update_directory_entry(const char* filename, FatEntry *start_cluster, uint32_t file_size);

  // if error {
  //   return error and -1
  // }

  // return 0 for success
}


THESE FUNCTIONS WILL USE DirectoryEntry:
DirectoryEntry* f_opendir(char* directory) { // Returns pointer to open directory
  // Open directory to read using directory
  // return DirectoryEntry;
}

DirectoryEntry* f_readdir(char* directory) { // Returns pointer to next directory
  // Read next directory entry
  // return DirectoryEntry;
}

int f_closedir(char* path) { // Returns int success or not
  // Close the open directory
  // if error {
  //   return error and -1
  // }

  // return 0 for success
}

int f_mkdir(char* path) { // Returns int success or not
  // create new directory at specified path
  
  // if error {
  //   return error and -1
  // }

  // return 0 for success
}

int f_rmdir(char* path) { // Returns int success or not
  // Checking if path exists
  // if it does not exist {
  //   return -1
  // }

  // all_entries = for the dir find all entries within it (files and other directories)
  
  // for each entry in all_entries {
  //   Check if entry is file or directory
  //   if file {
  //     remove file/path to file

  //     if error {
  //       return error and -1
  //     }
  //   } else if directory {
  //     call f_rmdir on entry/directory
  //     // Recursion to make sure all contents in dir/path is deleted

  //     if error {
  //       return error and -1
  //     }
  //   }
  // }

  // return 0 for success
}
