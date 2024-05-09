# file-system-implementation

format.c
====================================================================================
compile: gcc -o format format.c
run: ./format <filename> 
  (ex: ./format DISK)
run with specified size: format <filename> -s <num MB requested> 
  (ex: ./format DISK -s 2)

output: a disk image called <filename> will be created in your current directory
        on first run (if disk image/file does not already exist) format will create a          new empty disk, populated root dir with "/" and "."
        format.c does not yet suppport an existing disk image/file
        
void fs_mount(char *diskname)
====================================================================================
params: <filename> of disk image to mount in working directory  (ex: DISK)
return: void
side-effects: reads/prints/defines the following variables/structs:  superblock
                                                                     FAT
                                                                     bitmap/freemap
                                                                     rootdir entry
                                                                     datablock section                                                                       (0 block                                                                               reserved for                                                                           root dir)
mount works for any disk formatted with format.c, including a fake disk that works with pre-existing files/directories


filesystem.c
====================================================================================
gcc -o filesystem filesystem.c

<!-- With fake_disk: -->
<!-- FAT:
      0 | 0 |
      1 | 5 |
      2 | 0 |
      3 | 4 |
      4 | 0 |
      5 | 0 |
      6 |   |
-->

<!-- Data Section:
      ______________________________________________________________________
      | folder 1, folder 1, file1.txt | folder_a | EOF | Hello | EOF | EOF |
      ----------------------------------------------------------------------
                      0                     1       2      3      4      5
 -->