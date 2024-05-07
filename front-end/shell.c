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

// temp includes for front end testing (will use our own FS library later)
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

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
char my_pwd(void) {
    // output absolute address to terminal
    // also returns the char of path to current working directory
}

// creates a directory
int my_mkdir(const char *path, char **args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// removes a directory
int my_rmdir(const char *path, char **args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// deletes a file
int my_rm(const char *path, char **args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// displays the content of one or more files to the output
void my_cat(const char *path, char **args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // output file content to terminal
}

// lists a file a screen at a time
void my_more(const char *path, char **args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // output to terminal
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
        printf("command: ls\n");
        printf("l flag: %d\n", ls_l_flag);
        printf("F flag: %d\n", ls_F_flag);
        my_ls(&args, ls_l_flag, ls_F_flag);
        // TODO: support redirection
    } else if (strcmp(args[0], "cd") == 0) {
        printf("command: cd\n");
        // char path = my_pwd();
        // my_cd(path, args + 1);
    } else if (strcmp(args[0], "pwd") == 0) {
        // my_pwd();
        printf("command: pwd\n");
        // TODO: support redirection
    } else if (strcmp(args[0], "mkdir") == 0) {
        printf("command: mkdir\n");
        // char path = my_pwd();
        // my_mkdir(path, args + 1);
    } else if (strcmp(args[0], "rmdir") == 0) {
        printf("command: rmdir\n");
        // char path = my_pwd();
        // my_rmdir(path, args + 1);
    } else if (strcmp(args[0], "rm") == 0) {
        printf("command: rm\n");
        // char path = my_pwd();
        // my_rm(path, args + 1);
    } else if (strcmp(args[0], "cat") == 0) {
        printf("command: cat\n");
        // char path = my_pwd();
        // my_cat(path, args + 1);
        // TODO: support redirection
    } else if (strcmp(args[0], "more") == 0) {
        printf("command: more\n");
        // char path = my_pwd();
        // my_more(path, args + 1);
        // TODO: support redirection
    } else if (strcmp(args[0], "chmod") == 0) {
        printf("command: chmod\n");
        // char path = my_pwd();
        // my_chmod(path, args + 1);

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
    // TODO: call fs_mount to mount on our own disk
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
