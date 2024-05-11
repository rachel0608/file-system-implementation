// compile: gcc shell.c -o shell -lreadline

#define _POSIX_SOURCE 199309L
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>

#define FALSE 0
#define TRUE 1
#define FAIL -1
#define MAX_INPUT_SIZE 1024
#define DELIMITERS " ;\t\n"
#define MAX_PATH_LENGTH 1024
#define MAX_DIR_LENGTH 8
#define MAX_EXT_LENGTH 3

// temp includes for front end testing (will use our own FS library later)
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#define CAT_BUFFER_SIZE 1024
#define BUFFER_SIZE 1024
#define PAGE_SIZE 20

int interrupted = 0;

void handle_sigint(int sig) {
    interrupted = 1;
}

// lists all the files in the current or specified directory
void my_ls(char **args, int l_flag, int F_flag) {
    char *path = ".";  // Default to current directory

    if (args[1] != NULL) {
        path = args[1];  // Use the provided directory path
    }

    printf("entered path: %s\n", path);

    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // TODO: handle flags
    
    // Read and print each directory entry
    while ((entry = readdir(dir)) != NULL) {
        // Ignore current and previous directory (. and ..)
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    // Close the directory
    closedir(dir);
}

void my_cd(const char *path, char **args) {
    // check if args[1] contain “.” or “..”
    // if no arg, go to root directory
    // else check valid args: 
    // if len(args[1]) > 8, return -1 (name too long)
    // if given dir not found, return -1, print error
    // else, go to specified directory 
    // no return
}

// prints the current working directory to terminal
void my_pwd(void) {
    char current_working_dir[MAX_PATH_LENGTH];
    
    if (getcwd(current_working_dir, sizeof(current_working_dir)) != NULL) {
        printf("%s\n", current_working_dir);
    } else {
        perror("getcwd() error");
    }
}

// creates a directory
int my_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkdir: missing args\n");
        return -1;
    }

    if (strlen(args[1]) > MAX_DIR_LENGTH) {
        fprintf(stderr, "mkdir: directory name too long\n");
        return -1;
    }

    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;  // Set directory permissions

    if (mkdir(args[1], mode) == -1) {
        perror("mkdir");
        return -1;
    }

    return 1;
}

// removes a directory
int my_rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rmdir: missing operand\n");
        return -1;
    }

    if (strlen(args[1]) > MAX_DIR_LENGTH) {
        fprintf(stderr, "rmdir: directory name too long\n");
        return -1;
    }

    if (rmdir(args[1]) == -1) {
        perror("rmdir");
        return -1;
    }

    return 1;
}

// deletes a file


int my_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rm: missing operand\n");
        return -1;
    }

    if (remove(args[1]) == -1) {
        perror("rm");
        return -1;
    }

    return 1;
}

// displays the content of one or more files to the output
int my_cat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cat: missing operand\n");
        return -1;
    }

    int fd = open(args[1], O_RDONLY);
    if (fd == -1) {
        perror("cat");
        return -1;
    }

    char buffer[CAT_BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, CAT_BUFFER_SIZE)) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    if (bytes_read == -1) {
        perror("cat");
        close(fd);
        return -1;
    }

    close(fd);
    return 1;
}

// lists a file a screen at a time
int my_more(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "more: missing operand\n");
        return -1;
    }

    int fd = open(args[1], O_RDONLY);
    if (fd == -1) {
        perror("more");
        return -1;
    }

    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "more: memory allocation failed\n");
        close(fd);
        return -1;
    }

    ssize_t bytes_read;
    int line_count = 0;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            write(STDOUT_FILENO, &buffer[i], 1);
            if (buffer[i] == '\n') {
                line_count++;
                if (line_count == PAGE_SIZE) {
                    printf("--More--");
                    getchar();
                    line_count = 0;
                }
            }
        }
    }

    if (bytes_read == -1) {
        perror("more");
        free(buffer);
        close(fd);
        return -1;
    }

    free(buffer);
    close(fd);
    return 1;
}

// changes the permissions mode of a file
// supports absolute mode and symbolic mode
int my_chmod(const char *path, char **args) {
    // args[1] = mode & permission details (parse args[1] to set mode)
    // args[2] = file name
    // return -1 if fail
    // return 1 if done
}

// function to translate relative address to absolute
char *relative_to_absolute(char *relative_path, char *current_directory) {
    // return a char of absolute address
}

void child_handler(int signum, siginfo_t *info, void *context){

    pid_t pid = info -> si_pid; 
    //si_code in the info struct contains if the child is suspended, exited, or interrupted...
 	
   	if(info->si_code == CLD_EXITED){
        // printf("(Exited)\n");
        int status;
        waitpid(pid, &status, WNOHANG);
    }
    if(info->si_code== CLD_KILLED){
        // avoid zombie process
        int status;
        // printf("(Killed)\n");
        waitpid(pid, &status, WNOHANG);
    }
    if(info->si_code == CLD_CONTINUED){
        // printf("(Resumed)\n");
        struct termios setting;
	}
    if(info->si_code == CLD_STOPPED){
        kill(pid, SIGSTOP);
        // printf("(Suspended)\n");
   	}
}

// Update of previously established, working execute_command()
void execute_command(char *command_line) {
    // check for exit command (existing code)
    if (strcmp(command_line, "exit") == 0) {
        free(command_line);
        // TODO: free other global var if needed
        exit(0);
    }

    // vars to parse command line into command and arguments
    char *token = strtok(command_line, DELIMITERS);
    char *args[MAX_INPUT_SIZE];
    int arg_count = 0;
    int ls_l_flag = FALSE;
    int ls_F_flag = FALSE; 

    // variables for redirection handling
    char *input_file = NULL;
    char *output_file = NULL;
    int append_mode = FALSE;

    // check for empty command (existing code)
    if (token == NULL) {
        return; 
    }

    char entire_command[MAX_INPUT_SIZE];
    strcpy(entire_command, token);

    // tokenize input
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            // input redirection
            token = strtok(NULL, DELIMITERS);
            if (token != NULL) {
                input_file = token;
            }
        } else if (strcmp(token, ">") == 0) {
            // output redirection
            token = strtok(NULL, DELIMITERS);
            if (token != NULL) {
                output_file = token;
                append_mode = FALSE;
            }
        } else if (strcmp(token, ">>") == 0) {
            // append redirection
            token = strtok(NULL, DELIMITERS);
            if (token != NULL) {
                output_file = token;
                append_mode = TRUE;
            }
        } else if (strcmp(token, "-l") == 0 && strcmp(args[0], "ls") == 0) {
            // -l flag for ls command
            ls_l_flag = TRUE;
        } else if (strcmp(token, "-F") == 0 && strcmp(args[0], "ls") == 0) {
            // -F flag for ls command
            ls_F_flag = TRUE;
        }
        else {
            args[arg_count++] = token;
        }

        token = strtok(NULL, DELIMITERS);
    }
    args[arg_count] = NULL;

    // handle built-in commands
    if (strcmp(args[0], "ls") == 0) {
        printf("l flag: %d\n", ls_l_flag);
        printf("F flag: %d\n", ls_F_flag);
        my_ls(&args, ls_l_flag, ls_F_flag);
        // TODO: support redirection and flags
    } else if (strcmp(args[0], "cd") == 0) {
        printf("command: cd\n");
        // my_cd(path, args + 1);
    } else if (strcmp(args[0], "pwd") == 0) {
        my_pwd();
        // TODO: support redirection
    } else if (strcmp(args[0], "mkdir") == 0) {
        my_mkdir(&args);
    } else if (strcmp(args[0], "rmdir") == 0) {
        my_rmdir(&args);
    } else if (strcmp(args[0], "rm") == 0) {
        my_rm(&args);
    } else if (strcmp(args[0], "cat") == 0) {
        my_cat(&args);
        // TODO: support redirection
    } else if (strcmp(args[0], "more") == 0) {
        printf("command: more\n");
        my_more(&args);
        // TODO: support redirection
    } else if (strcmp(args[0], "chmod") == 0) {
        printf("command: chmod\n");
        // my_chmod(&args);
    } else {

        // start a child process to execute normal command
        pid_t pid = fork();

        if (pid == 0) { // child

            // set ignored signals to default (existing code)
            signal(SIGINT,SIG_DFL);
            signal(SIGTSTP,SIG_DFL);
            signal(SIGTERM,SIG_DFL);
            signal(SIGTTIN,SIG_DFL);
            signal(SIGTTOU,SIG_DFL);
            signal(SIGQUIT,SIG_DFL);

            // handle input redirection
            // TODO: change to our own open/close
            if (input_file != NULL) {
                printf("input_file: %s\n", input_file);

                int input_fd = open(input_file, O_RDONLY);
                if (input_fd == -1) {
                    perror("open input file");
                    exit(EXIT_FAILURE);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            // handle output redirection
            if (output_file != NULL) {
                printf("output_file: %s\n", output_file);
                printf("append_mode: %d\n", append_mode);
                int output_fd;
                if (append_mode) {
                    output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                if (output_fd == -1) {
                    perror("open output file");
                    exit(EXIT_FAILURE);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }

            // execute command
            execvp(args[0], args);
            perror("execvp fails");
            exit(EXIT_FAILURE);

        } else if (pid > 0){  // parent 

            int status;
            if (waitpid(0, &status, WUNTRACED) == -1) {
                perror("waitpid");
            }

        } else { // bad fork
            perror("fork fails");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    char *input = (char *)NULL;

    // ignore useless signals
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGTERM,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGQUIT,SIG_IGN);

    struct sigaction chldsa;
    chldsa.sa_sigaction = child_handler;
    sigemptyset(&chldsa.sa_mask);
	// block sigchld signals during the execution of the sigchld handler
	sigaddset(&chldsa.sa_mask, SIGCHLD); 
    chldsa.sa_flags = SA_RESTART|SA_SIGINFO;

    if (sigaction(SIGCHLD, &chldsa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    // main loop
    // TODO: call fs_mount to mount on our own disk: fs_mount(<disk_name>)
    
    while (1) {
        // reset or empty input each time
        if (input){
            free(input);
            input = (char *)NULL;
        }

        // get command

        input = readline("${username}> "); // TODO: keep track of username
        
        if (!input){
            printf("\n");
            break;
        }
        
        if (*input){
            // save current fd (both std in and std out)
            execute_command(input);
            // restore fd (both std in and std out)
        }
    }
    
    free(input);
    input = (char *)NULL;

    return 0;
}
