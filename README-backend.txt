README for filesystem (back end) ===============================================

NAME:
=====
    Grace Choe, Cecilia Zhang, Julia Rieger, and Rachel Nguyen

* Important Note:
    The testing we've kept are all for fake_disk_1.
    However, all the implemented features also work for fake_disk_2. 

    test_remove_disk_1() and test_removedir_disk_1() in main() should be uncommented 
    and run individually

Program Files:
==============
    For formatting the disk:
        format.c

    For the filesystem:
        filesystem.h
        filesystem.c
        fake_disk_1.c
        fake_disk_2.c

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

    test_remove_disk_1() and test_removedir_disk_1() in main() should be uncommented 
    and run individually

* Can also compile each fake_disk individually using: ./fake_disk_1
                                                      ./fake_disk_2

Implemented Features:
=====================
    Part 1. Working Commands:
        - f_open        (does not include permission check / creating new file)
        - f_read        (assumes file position is 0)
        - f_close       (no valgrind errors)
        - f_opendir
        - f_readdir
        - f_closedir    (no valgrind errors)
        - f_seek
        - f_rewind
        - f_remove()

    Part 2. Partially Working/Created but Not Fully Working Commands:
        - f_write()

    Part 3. Not Implemented:
        - f_stat()
        - f_mkdir()
        - f_rmdir()

How Testing Was Done: (Used Print/Testing Statements to Terminal)
=================================================================
IMPORTANT NOTES
* Our testing print statements contains print statments for all the traversals
  to show our filesystem functions work and are NOT hardcoded.
* To shorten this README, these prints are truncated.
* Please run ./fake_disk_1 everytime before running tests to ensure consistency, 
  as some tests modifies contents in fake disks. 


* Running the ./filesystem will show the tests in its entirety.
    === tests included in test_opendir_readdir_disk_1() ===
    - f_opendir (prints out the directory entry returned)
        1. Open root directory
        Filename: /
        Extension:
        First logical cluster: 0

        2. Open /Desktop directory
        Filename: Desktop
        Extension:
        First logical cluster: 1

        3. Open /Download directory
        Filename: Download
        Extension:
        First logical cluster: 65535 (empty)

        4. Attempt to open /Hello.txt (not a directory)
        f_opendir: cannot find given dir, Hello.txt.
        Cannot open Hello.txt because it is not a directory.

        5. Open /Desktop/CS355 directory
        Filename: CS355
        Extension:
        First logical cluster: 4

        6. Open /Desktop/CS355/labs/lab1
        Filename: lab1
        Extension:
        First logical cluster: 65535

        7. Attempt to open /Desktop/CS355/labs/lab1/essay (non-existent)
        readdir: given entry is empty.
        f_opendir: cannot find given dir, essay.
        Cannot open essay because it does not exist.

    - f_readdir (prints out all directory entries returned)
        1. Read root directory
        Entry 0 Desktop
        Entry 1 Download
        Entry 2 Hello

        2. Read /Desktop directory
        Entry 0 CS355
        Entry 1 blog_1

        3. Read /Download directory
        readdir: given entry is empty.

        4. Read /CS355 directory
        Entry 0 hw1
        Entry 1 labs

        5. Read /labs/lab1 directory
        readdir: given entry is empty.

        6. Read /labs/lab1/essay directory (non-existent)
        readdir: given NULL entry.

        7. Read /Hello.txt (not a directory)
        readdir: given NULL entry.

    - f_closedir (prints out success or expected fail)
        1. Close root directory
        closing entry: /
        close success~

        2. Close /Desktop directory
        closing entry: Desktop
        close success~

        3. Close /Download directory
        closing entry: Download
        close success~

        4. Close /CS355 directory
        closing entry: CS355
        close success~

        5. Close /labs/lab1 directory
        closing entry: lab1
        close success~

        6. Close /labs/lab1/essay directory (non-existent)
        closedir: given NULL. 
        close failed!!!
        Fail expected -- Null entry

        7. Close /Hello.txt (not a directory)
        closedir: given NULL. 
        close failed!!!
        Fail expected -- Null entry 


    === tests included in test_open_read_disk_1() ===
    - f_open (prints out the file handle returned)
        1. Open /Desktop/blog_1.txt
        Absolute Path: /Desktop/blog_1.txt
        First Logical Cluster: 3
        File Descriptor: 0
        File Access Type: r
        File Position: 0
        File Size: 56

        2. Attempt to open /Desktop/blog_2.txt (non-existent)
        ERROR: File does not exist.
        
        3. Attempt to open /Desktop/CS355 (folder)
        ERROR: Cannot open a directory.

        4. Open /Desktop/CS355/hw1.txt
        Absolute Path: /Desktop/CS355/hw1.txt
        First Logical Cluster: 5
        File Descriptor: 0
        File Access Type: r
        File Position: 0
        File Size: 9

        5. Open /Hello.txt
        Absolute Path: /Hello.txt
        First Logical Cluster: 2
        File Descriptor: 0
        File Access Type: r
        File Position: 0
        File Size: 7

    - f_read (prints read bytes/content in the file)
        1. Read /Desktop/blog_1.txt -> blog_1.txt contains "This is blog 1... Blog 1 is also here! Blog 1 ends here!"
        Successfully read 56 bytes from the file.
        Bytes read: 56
        Buffer: This is blog 1... Blog 1 is also here! Blog 1 ends here!
        
        2. Read /Desktop/CS355/hw1.txt -> hw1.txt contains "1 + 1 = 2"
        Successfully read 9 bytes from the file.
        Bytes read: 9
        Buffer: 1 + 1 = 2

        3. Read /Desktop/blog_2.txt (non-existent)
        Error: File handle is NULL.
        Bytes read: -1
        Buffer:

        4. Read /Desktop/CS355 (folder - should not read)
        Error: File handle is NULL.
        Bytes read: -1
        Buffer:

        5. Read /Hello.txt -> Hello.txt contains "Hello~!"
        Successfully read 7 bytes from the file.
        Bytes read: 7
        Buffer: Hello~!

        6. Read from NULL buffer
        Error: Buffer pointer is NULL.
        Bytes read: -1

    - f_seek (prints the new file position location)
        1. Change File Position Location for /Desktop/blog_1.txt
            a. Check with SEEK_SET
            Expected Updated File Position: 1
            Successfully fseek()’ed
            Updated File Position: 1

            b. Check with SEEK_CURR
            Expected Updated File Position: 2
            Successfully fseek()’ed
            Updated File Position: 2

            c. Check with SEEK_END
            Expected Updated File Position: 56
            Successfully fseek()’ed
            Updated File Position: 56

    - f_rewind (prints the new file position location)
        1. Rewind File Position Location for /Desktop/blog_1.txt
        Expected Updated File Position: 0
        Rewinded File Position: 0

    - f_close
        1. Close /Desktop/blog_1.txt
        file found. freeing file...
        close success~

        2. Attempt to close /Desktop/blog_2.txt (non-existent)
        f_close: NULL file.
        close failed :(
        Fail expected -- Null file

        3. Attempt to close /Desktop/CS355 (folder)
        f_close: NULL file.
        close failed :(
        Fail expected -- Null file

        4. Close /Desktop/CS355/hw1.txt
        file found. freeing file...
        close success~

        5. Close /Hello.txt
        file found. freeing file...
        close success~

    === tests included in test_remove_disk_1() ===
    - f_remove()
        1. Remove /Hello.txt
        dir: /   file: Hello.txt
        ...
        Start finding file...
        Marking the directory entry as unused...
        Remove success. Expected~
        Checking with f_open(). Expected to print ERROR: File does not exist
        ...
        ERROR: File does not exist.
        NOTE: Current version of f_open() cannot handle new file creation.

        2. Remove /Desktop (directory, should fail)
        dir: /   file: Desktop
        Directory opened: /
        Start finding file...
        f_remove: File /Desktop not found
        Remove failed. Expected~

        3. Remove /fake_file.txt (non-existent)
        dir: /   file: fake_file.txt
        Directory opened: /
        Start finding file...
        f_remove: File /fake_file.txt not found
        Remove failed. Expected~

        4. Remove /Desktop/blog_1.txt
        dir: /Desktop   file: blog_1.txt
        ...
        Start finding file...
        Marking the directory entry as unused...
        Remove success. Expected~
        Checking with f_open(). Expected to print ERROR: File does not exist
        ...
        ERROR: File does not exist.
        NOTE: Current version of f_open() cannot handle new file creation. 

    === tests included in test_rmdir_disk_1() ===
    - f_rmdir()
        1. Remove /Essay (non-existent)
        Looking up directory: Essay...
        Reading directory entries for '/'...
        ...
        f_opendir: cannot find given dir, Essay. 
        f_rmdir: Directory /Essay not found
        Remove failed as expected.

        2. Remove /Desktop/CS355/labs/lab1 (labs/ should be empty afterwards)
        Looking up directory: Desktop...
        ...
        Looking up directory: CS355...
        ...
        Looking up directory: labs...
        ...
        Found: labs
        Looking up directory: lab1...
        ...
        Found: lab1
        Successfully opened directory ---
        f_rmdir: Directory /Desktop/CS355/labs/lab1 removed
        f_rmdir: Finding parent directory of /Desktop/CS355/labs/lab1...
        Looking up directory: Desktop...
        ...
        Looking up directory: CS355...
        ...
        Looking up directory: labs...
        ...
        Found: labs
        Successfully opened directory ---
        readdir: empty directory entries. Returning NULL.
        f_rmdir: Parent directory /Desktop/CS355/labs/ is now empty
        === Directory Entry Info === 
        Filename: labs
        Extension: 
        First logical cluster: 65535 (empty)
        Successfully removed /Desktop/CS355/labs/lab1

        3. Remove /Desktop/CS355 (not empty)
        Looking up directory: Desktop...
        ...
        Looking up directory: CS355...
        ...
        Found: CS355
        Successfully opened directory ---
        f_rmdir: Directory /Desktop/CS355 is not empty
        Remove failed as expected.

        4. Remove /Desktop/CS355/blog_1.txt (file)
        Looking up directory: Desktop...
        ...
        Looking up directory: CS355...
        ...
        Looking up directory: blog_1.txt...
        ...
        f_opendir: cannot find given dir, blog_1.txt. 
        f_rmdir: Directory /Desktop/CS355/blog_1.txt not found
        Remove failed as expected.

        5. Remove root directory
        Directory opened: /
        f_rmdir: Directory / is not empty
        Remove failed as expected.

        6. Remove /Download (directory)
        Looking up directory: Download...
        ...
        Found: Download
        Successfully opened directory ---
        f_rmdir: Directory /Download removed
        f_rmdir: Finding parent directory of /Download...
        Directory opened: /
        Reading directory entries for '/'...
        Entry 0 Desktop
        f_rmdir: Parent directory's remaining subdirs
        === Directory Entry Info === 
        Filename: Desktop
        Extension: 
        First logical cluster: 1
        === Directory Entry Info === 
        Filename: Hello
        Extension: txt
        First logical cluster: 2
        ===========
        Successfully removed /Download

    === tests included in test_mkdir_disk_1() ===
    - f_mkdir()
        1. Make directory /Desktop/CS355/labs/lab2 (labs/ is not empty)
        Looking up directory: Desktop...
        ...
        Looking up directory: CS355...
        ...
        Looking up directory: labs...
        ...
        Found: labs
        Successfully opened directory ---
        Reading directory entries for 'labs'...
        Entry 0 lab1
        Found empty directory entry at index 1
        Directory lab2 created in /Desktop/CS355/labs/
        === Directory Entry Info === 
        Filename: lab2
        Extension: 
        First logical cluster: 65535 (empty)
        Parent directory's info
        Reading directory entries for 'labs'...
        Entry 0 lab1
        Entry 1 lab2
        === Directory Entry Info === 
        Filename: lab1
        Extension: 
        First logical cluster: 65535 (empty)
        === Directory Entry Info === 
        Filename: lab2
        Extension: 
        First logical cluster: 65535 (empty)
        ===========
        Successfully created /Desktop/CS355/labs/lab2

        2. Make directory /Desktop/CS355 (already exists)
        Looking up directory: Desktop...
        ...
        Found: Desktop
        Successfully opened directory ---
        Reading directory entries for 'Desktop'...
        Entry 0 CS355
        Entry 1 blog_1
        ERROR: Directory already exists.
        Make directory failed as expected.

        3. Make directory /Download/Images (Download/ is empty)
        Looking up directory: Download...
        Reading directory entries for '/'...
        ...
        Found: Download
        Successfully opened directory ---
        readdir: given entry is empty.
        Parent directory is empty.
        Found empty cluster at index 9
        Directory Images created in /Download/
        === Directory Entry Info === 
        Filename: Images
        Extension: 
        First logical cluster: 65535 (empty)
        Parent directory's info
        Reading directory entries for 'Download'...
        Entry 0 Images
        === Directory Entry Info === 
        Filename: Images
        Extension: 
        First logical cluster: 65535 (empty)
        ===========
        Successfully created /Download/Images


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
