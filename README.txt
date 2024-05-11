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
        This is program to implement functions for the shell (front end).

    filesystem.h:
        This is a header file for the filesystem.
    
    filesystem.c:
        This is program to implement functions for the filesystem (back end).
    
    format.c:
        This is program to format the disk.
    
    fat.h:
        This is the header for FAT.
    
    fat.c:
        This is the implementation for FAT.
    
    fake_disk_1.c:
        This program contains a fake disk.
    
    fake_disk_2.c:
        This program contains another fake disk.

    Makefile
        This file contains specfics on how to compile the .c file into the
        specified executable.

    README.txt
        Contains the text you see here.
    
    README-backend.txt
        Contains more information about the backend files and how to run them.
        Contains tests and details on which functions work, are partially working,
        and have not been implemented.

    README-frontend.txt
        Contains more information about the frontend files and how to run them.
        Contains tests and details on which functions work, are partially working,
        and have not been implemented.

    disks (folder)
        disk_layout.txt
            Contains information on how each fake disk is laid out.
        
        fake_disk_1.img
        fake_disk_2.img
            Contains the fake_disks .img

How to Clean:
=============
    Removes everything:
      make clean
