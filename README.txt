README for everything ==========================================================

NAME:
=====
    Cecilia Zhang, Grace Choe, Julia Rieger, and Rachel Nguyen

-- * IMPORTANT NOTE -------------------------------------------------------------
|  For an in depth summary of features and a step by step guide to testing our  |
|  implemented features go to the:                                              |
|                                                                               |
|    README-frontend.txt -> for testing the frontend                            |
|    README-backend.txt -> for testing the backend                              |
---------------------------------------------------------------------------------

Program Files:
==============
    Part 1 - Frontend:
        shell.c

    Part 2 - Backend:
        For formatting the disk:
        format.c

        For the filesystem:
            filesystem.h
            filesystem.c
            fake_disk_1.c
            fake_disk_2.c
            fat.h
            fat.c
            /disks - Folder containing all the compiled fake disks
                    (fake_disk_1.img & fake_disk_2.img) which gets updated when
                    make fake_disks is called.
   
   Part 0 - Fake Test Disks:
        fake_disk_1.c
        fake_disk_2.c
	
How to Compile:
===============
    For compiling all files (can use make OR make all):
        make
    Part 1 - Frontend (can use make frontend OR make shell):
        make frontend -> Compiles everything in frontend
        make shell

    Part 2 - Backend (can use make backend OR compile each part individually):
    * IMPORTANT NOTE -> Fake disks must be compiled/make'd before filesystem
        make backend -> Compiles everything in backend
            OR (do the below in the order listed)
        make format
        make fake_disks -> Compiles all fake disks
        make fake_disk_1 -> Compiles fake_disk_1.c
        make fake_disk_2 -> Compiles fake_disk_2.c
        make filesystem
   
   Part 0 - Fake Test Disks:
        make fake_disks -> Compiles all fake disks
            OR
        make fake_disk_1 -> Compiles fake_disk_1.c
        make fake_disk_2 -> Compiles fake_disk_2.c

How to Run:
===========
    Part 1 - Frontend:
        ./shell

    Part 2 - Backend (do the below in the order listed):
        make run_disks
        ./filesystem
            OR
        ./fake_disk_1
        ./fake_disk_2
        ./filesystem

        format.c:
            To run:
                ./format <filename> -> ex: ./format DISK
            To run with specified size:
                ./format <filename> -s <num MB requested>  -> ex: ./format DISK -s 2
	
Known Bugs or Limitations:
==========================
    The filesystem is not fully implemented. The front and backend are not
    integrated, thus, we will be showing how each part implemented so far can
    work and run by itself. Look at the section below for more information on
    the parts implemented.

Summary of Features and EC Completed:
=====================================
    For an in depth summary of features and a step by step guide to testing our
    implemented features go to the:
        README-frontend.txt -> for testing the frontend
        README-backend.txt -> for testing the backend

    Part 1 - Frontend:
        shell.c

    Part 2 - Backend:
        filesystem.h
        filesystem.c
        format.c
        fat.h
        fat.c
   
   Part 0 - Fake Test Disks:
        fake_disk_1.c
        fake_disk_2.c

File Directory:
===============
    shell.c:
      This is a header file that contains structs for superblock and inode and
      other function declarations for defrag.c.

    filesystem.h:
    
    filesystem.c:
    
    format.c:
    
    fat.h:
    
    fat.c:
    
    fake_disk_1.c:
    
    fake_disk_2.c:
      This program is an implementation of a disk defragmenter for a Unix-like
      (inode-based) file system. File system defragmenters improve performance
      by compacting all the blocks of a file into sequential order on disk.

    datafile-frag:
      The given fragmented disk file for testing (now in the datafiles folder).

    Makefile
      This file contains specfics on how to compile the .c file into the
      specified executable.

    README.txt
      Contains the text you see here.

    datafiles (folder)
      A folder full of fragmented disk files for resubmission testing.

      The fragmented disk files in the folder:
        datafile-frag
        1024-direct
        1024-empty
        blocksize
        empty
        freelist
        large
        large-frag-withfree
        larger-frag-withfree
        medium
        medium2
        small
        verify-defragmentation -> used to verify the defragmentation

How to Clean:
=============
    Removes defrag:
      make clean
