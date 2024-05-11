README for shell (front end) ===================================================

NAME:
=====
    Grace Choe, Cecilia Zhang, Julia Rieger, and Rachel Nguyen

Program Files:
==============
    shell.c

How to Compile:
===============
    make frontend
        OR
    make shell

How to Run:
===========
    ./shell

Implemented Features & How to Test:
===================================
    Part 1. Built-in commands (no redirection support):
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
            tested: more <filename>
            note: no support for redirection. press "enter" for next page. 
        - rm
            tested: rm <filename>

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

How to Clean:
=============
    Removes everything:
      make clean