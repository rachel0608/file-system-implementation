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
    Part 1. Commands:
        - f_open:
        - f_close:
        - f_seek:
        - f_rewind:
        - f_opendir:
        - f_closedir:
        - f_read:

        - ls 
            tested: ls, ls <relative path>, ls <path>
            note: no support for flags or redirection. 
        - pwd
            tested: pwd
            note: no support for redirection.
        - cd
            tested: cd <relative path>, cd <path>, cd, cd ., cd ..
            note: use pwd after cd to check if working directory is changed properly
        - mkdir
            tested: mkdir <dir_name>, mkdir (missing args)
            note: us ls after mkdir to check
        - rmdir
            tested: rmdir <dir_name>, rmdir (missing operand)
            note: us ls after mkdir to check
        - cat
            tested: cat <filename>
            note: no support for redirection. Needs args, can only cat 1 file at a time. 
        - more
            test: more <filename>
            note: no support for redirection. press "enter" for next page. 
        - rm
            test: rm <filename>

    Part 2. Non built-in commands with redirection:
        - input (<)
            cat < test.txt
        - output (>)
            echo hi > test.txt
            ps > ps.txt
        - append (>>)
            echo hiii >> test.txt
            ps >> ps.txt

    Part 3. Not implemented:
        - -l and -F flags for ls
        - chmod (only has parser)
        - redirection support for ls, pwd, cat, more
        - integration with our own filesystem backend

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