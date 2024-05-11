README for filesystem (back end) ===============================================

NAME:
=====
    Grace Choe, Cecilia Zhang, Julia Rieger, and Rachel Nguyen

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

Implemented Features:
=====================
    Part 1. Working Commands:
        - f_open
        - f_read
        - f_opendir
        - f_readdir
        - f_seek
        - f_rewind

    Part 2. Created but Not Working Commands:
        - f_close
        - f_closedir

    Part 3. Not Implemented:
        - f_write()
        - f_stat()
        - f_remove()
        - f_mkdir()
        - f_rmdir()
        - f_mount() (Extra Credit not done)
        - f_umount() (Extra Credit not done)

How to Test:
============

Limitations:
============
    Shell does not work with our own file system (back end). They are not yet
    integrated together.
    All built-in commands used the Linux system calls. It does not work on our
    own disk, hence we did not mount shell before launching the main loop.

Other Useful Notes:
===================
    We tried our best~!

How to Clean:
=============
    Removes everything:
      make clean