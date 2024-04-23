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
#define MAX_INPUT_SIZE 1024
#define DELIMITERS " ;\t\n"

// for shell command implementation, see folder: shell_commands

// lists all the files in the current or specified directory
void my_ls(char *args, int l_flag, int F_flag) {
    char *path = args[1];

    // handle no args
    if (args[1] == NULL) {
        path = my_pwd();
    }

    // output list of files to terminal
}

void my_cd(const char *path, char *args) {
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
int my_mkdir(const char *path, char *args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// removes a directory
int my_rmdir(const char *path, char *args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// deletes a file
int my_rm(const char *path, char *args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // return 1 when done
}

// displays the content of one or more files to the output
void my_cat(const char *path, char *args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // output file content to terminal
}

// lists a file a screen at a time
void my_more(const char *path, char *args) {
    // if no args, return -1 
    // if len(args[1]) > 8, return -1 (name too long)
    // if no args or given dir not found, return -1
    // output to terminal
}

// changes the permissions mode of a file
// supports absolute mode and symbolic mode
int my_chmod(const char *path, char *args) {
    // args[1] = mode & permission details (parse args[1] to set mode)
    // args[2] = file name
    // return -1 if fail
    // return 1 if done
}

// function to translate relative address to absolute
char *relative_to_absolute(const char *relative_path, const char *current_directory) {
    // return a char of absolute address
}

// Update of previously established, working execute_command()
void execute_command(char *command_line) {
    // check for exit command (existing code)
    if (strcmp(command_line, "exit") == 0) {
        free(command_line);
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
        args[arg_count++] = token;

        // check for redirection symbols
        if (token contains "<") {
	        // parse input redirection symbol

        } else if (token contains ">") {
	        // parse output redirection

        } else if (token contains ">>") {
            // parse append redirection

        } else if (token contains "ls") {
            // check for flags
            // set flags if needed

        } else {
            // parse commands normally (existing code)
            token = strtok(NULL, DELIMITERS);

            // concatenate tokens into entire_command
            if (token != NULL) {
                strcat(entire_command, " ");
                strcat(entire_command, token);
            }
        }
    } // end of while

    // start a child process to execute command
    pid_t pid = fork();

    if (pid == 0) { // child

        // set ignored signals to default (existing code)
        signal(SIGINT,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        signal(SIGTERM,SIG_DFL);
        signal(SIGTTIN,SIG_DFL);
        signal(SIGTTOU,SIG_DFL);
        signal(SIGQUIT,SIG_DFL);

        if (setpgid(0, 0) == -1) {
            perror("setpgid");
            exit(EXIT_FAILURE);
        }

        // decision tree to call corresponding functions for commands
        if(args[0] == "ls"){
            my_ls(args, ls_l_flag, ls_F_flag);
        } else if (args[0] == "cd"){
            char path = my_pwd();
            my_cd(path, args);
        } else if (args[0] == "pwd"){
            my_pwd();
        } else if (args[0] == "mkdir"){
            char path = my_pwd();
            my_mkdir(path, args);
        } else if (args[0] == "rmdir"){
            char path = my_pwd();
            my_rmdir(path, args);
        } else if (args[0] == "rm"){
            char path = my_pwd();
            my_rm(path, args);
        } else if (args[0] == "cat"){
            char path = my_pwd();
            my_cat(path, args);
        } else if (args[0] == "more"){
            char path = my_pwd();
            my_more(path, args);
        } else if (args[0] == "chmod"){
            char path = my_pwd();
            my_chmod(path, args);
        }
    } else if (pid > 0){  // parent 

        if (setpgid(pid, 0) == -1) {
            perror("setpgid");
            exit(EXIT_FAILURE);
        }


    } else { // bad fork
        perror("fork fails");
        exit(EXIT_FAILURE);
    }
}



int main() {
    char *input = (char *)NULL;

    // ignore useless signals (existing code)

    // ignore useless signals
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGTERM,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGQUIT,SIG_IGN);

    struct sigaction chldsa;
    chldsa.sa_sigaction = child_handler; // NOTE: will need to copy full code for this
    sigemptyset(&chldsa.sa_mask);
	// block sigchld signals during the execution of the sigchld handler
	sigaddset(&chldsa.sa_mask, SIGCHLD); 
    chldsa.sa_flags = SA_RESTART|SA_SIGINFO;

    if (sigaction(SIGCHLD, &chldsa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
  
    while (1) {
        // reset or empty input each time
        if (input){
            free(input);
            input = (char *)NULL;
        }

        // get command

        input = readline("${username}> "); //TODO: keep track of username
        
        if (!input){
            printf("\n");
            break;
        }
        
        updateJob();
        if (*input){
            // save current fd (both std in and std out)
            execute_command(input);
            // restore fd (both std in and std out)
        }
    }
  
    return 0;
}