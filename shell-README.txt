README for shell (front end)

How to compile: 
make shell

How to run: 
./shell

Implemented features and how to test:
a. built-in commands (no redirection support)
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

b. non built-in commands with redirection
    - input (<)
        cat < test.txt
    - output (>)
        echo hi > test.txt
        ps > ps.txt
    - append (>>)
        echo hiii >> test.txt
        ps >> ps.txt

Not implemented
    - -l and -F flags for ls
    - chmod (only has parser)
    - redirection support for ls, pwd, cat, more
    - integration with our own filesystem backend

Limitation
Shell does not work with our own file system (back end). 
All built-in commands used the Linux system calls. It does not work on our own disk, hence we did not mount shell before launching the main loop.  

Other useful notes:
    - use control + c to terminate current running process
    - use control + d or exit to stop shell