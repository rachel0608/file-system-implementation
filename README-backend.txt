README for filesystem (back end) ===============================================

NAME:
=====
    Cecilia Zhang, Grace Choe, Julia Rieger, and Rachel Nguyen

Program Files:
==============
    For formatting the disk:
        format.c

    For the filesystem:
        filesystem.h
        filesystem.c
        fake_disk_1.c
        fake_disk_2.c
        fat.h
        fat.c

How to Compile:
===============
* IMPORTANT NOTE -> Fake disks must be compiled/make'd before filesystem

    make backend -> Compiles everything in backend
        OR (do the below in the order listed)
    make fake_disks
    make filesystem

* Can also compile each fake_disk individually using: make fake_disk_1
                                                      make fake_disk_2

How to Run:
=========== (do the below in the order listed)
    make run_disks
    ./filesystem

* Can also compile each fake_disk individually using: ./fake_disk_1
                                                      ./fake_disk_2

Implemented Features & How to Test:
===================================
    Part 1. Working Commands:
        - f_open: open the specified file with the specified access. If the
                  file does not exist, handle accordingly. Returns a file
                  handle if successful.

        - f_read: read the specified number of bytes from a file handle at the
                  current position. Returns the number of bytes read, or an error.

        - f_close: close a file handle.

        - f_opendir: recall that directories are handled as special cases of
                     files. open a “directory file” for reading, and return a
                     directory handle.
        
        - f_readdir: returns a pointer to a “directory entry” structure
                     representing the next directory entry in the directory
                     file specified.

        - f_closedir: close an open directory file.

    Part 2. Not Working or Partially Working/Created Commands:
        - f_seek: move to a specified position in a file.
        - f_rewind: move to the start of the file.

    Part 3. Not Implemented:
        - f_write()
        - f_stat()
        - f_remove()
        - f_mkdir()
        - f_rmdir()
        - f_mount() (Extra Credit not done)
        - f_umount() (Extra Credit not done)

Limitations:
============
    Shell does not work with our own file system (back end). They are not yet
    integrated together.
    All built-in commands used the Linux system calls. It does not work on our
    own disk, hence we did not mount shell before launching the main loop.

Other Useful Notes:
===================
    - use control + c to terminate current running process
    - use control + d or exit to stop shell