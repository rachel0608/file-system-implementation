/*
 * mysh.c
 * 
 * Authors:
 *    Cecilia Chen, Cecilia Zhang, Paprika Chen
 */
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

#define MAX_INPUT_SIZE 1024
#define DELIMITERS " ;\t\n"

#define EXIT_FLAG 0
#define SUSPEND_FLAG 1
#define STOP_FLAG 2
#define RESUME_FLAG 3

// all possible job statuses
enum JobStatus {
    RUNNING,
    BLOCKED,
    READY
};

//  job struct using double linked list
struct Job {
    int jobId;
    int is_background;
    char command[MAX_INPUT_SIZE];
    enum JobStatus status;
    pid_t pid;
    struct termios setting;
    struct Job* next;
    struct Job* prev; 
};

struct Job* job_list = NULL; // head of job LL
struct Job* tail = NULL;
int jobCounter = 0; // total number of current jobs

// function to create a new job
struct Job* createJob(int is_background, const char* command, pid_t pid) {
    struct termios setting;
    if(tcgetattr(STDIN_FILENO, &setting)< 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    struct Job* newJob = (struct Job*)malloc(sizeof(struct Job));
    if (newJob == NULL) {
        perror("Error creating job");
        exit(EXIT_FAILURE);
    }

    // assign vars
    newJob->jobId = ++jobCounter; 
    newJob->is_background = is_background;
    strncpy(newJob->command, command, sizeof(newJob->command) - 1);
    newJob->status = RUNNING; // default = RUNNING
    newJob->pid = pid;
    newJob->setting = setting;
    newJob->next = NULL;
    newJob->prev = NULL;
    return newJob;
}

// function to add a job to the doubly linked list
void addJob(int is_background, const char* command, pid_t pid) {
    struct Job* newJob = createJob(is_background, command, pid);
    if (job_list == NULL) {
        // when list is empty
        job_list = newJob;
        tail = newJob; 
    } else {
        // when list is not empty
        tail->next = newJob;
        newJob->prev = tail;
        tail = newJob;
    }
}

// function to remove a job from the doubly linked list
void removeJob(pid_t pid) {
    struct Job* current = job_list;

    while (current != NULL && current->pid != pid) {
        current = current->next;
    }

    if (current == NULL) {
        printf("Job with ID %d not found\n", pid);
        return;
    }

    if (current->prev == NULL) {
        // If the job to be removed is the first one
        job_list = current->next;
        if (job_list != NULL) {
            job_list->prev = NULL;
        }
    } else {
        // If the job to be removed is not the first one
        current->prev->next = current->next;
        if (current->next != NULL) {
            current->next->prev = current->prev;
        }
    }

    if (current == tail) {
        // Update the tail if the removed job was the last one
        tail = current->prev;
    }

    // Update the jobid
    struct Job* update = current->next;
    while (update!=NULL){
        update->jobId--;
        update = update->next;
    }
    jobCounter--;
    free(current);

    printf("Job with PID %d removed\n", pid);
}

// function to print the doubly linked list
void printJobs() {
    struct Job* current = job_list;
    while (current != NULL) {
        printf("[%d]%d  %-20s %s\n", current->jobId, (int)current->pid,(current->status == RUNNING) 
        ? "Running" : (current->status == BLOCKED) ? "Blocked" : "Ready",current->command);
        current = current->next;
    }
}

// function to free the memory allocated for the doubly linked list
void freeJobList() {
    struct Job* current = job_list;
    while (current != NULL) {
        struct Job* temp = current;
        current = current->next;
        free(temp);
    }
}

struct Revise* revise_list = NULL;

// struct definition for the node in the linked list
struct Revise {
    pid_t pid;
    int flag;
    struct termios setting;
    struct Revise* next;
};

// function to create a new node with given flag and pid
struct Revise* createNode(pid_t pid, int flag) {
    struct Revise* newNode = (struct Revise*)malloc(sizeof(struct Revise));
    if (newNode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newNode->pid = pid;
    newNode->flag = flag;
    newNode->next = NULL;
    return newNode;
}

// function to insert a node at the end of the linked list
void add(struct Revise** head, pid_t pid, int flag) {
    struct termios setting;
    if(tcgetattr(STDIN_FILENO, &setting)< 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    struct Revise* newNode = createNode(pid, flag);
    newNode ->setting = setting;
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct Revise* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// function to remove the first node from the linked list
struct Revise* pop(struct Revise** head) {
    if (*head == NULL) {
        return NULL;
    }
    struct Revise* temp = *head;
    *head = (*head)->next;
    return temp;
}

// function to print the linked list
void printList(struct Revise* head) {
    struct Revise* temp = head;
    while (temp != NULL) {
        printf("PID: %d, Flag: %d\n", temp->pid, temp->flag);
        temp = temp->next;
    }
}

// function to free memory allocated for the linked list
void freeList(struct Revise* head) {
    struct Revise* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// parses and executes the given command line using a child process
void execute_command(char *command_line) {
    int run_in_background = 0;
    
    // check for exit command
    if (strcmp(command_line, "exit") == 0) {
        free(command_line);
        exit(0);
    }

    // parse command line into command and arguments
    char *token = strtok(command_line, DELIMITERS);
    char *args[MAX_INPUT_SIZE];
    int arg_count = 0;
    
    // check for empty command
    if (token == NULL) {
        return;
    }

    char entire_command[MAX_INPUT_SIZE];
    strcpy(entire_command, token);

    // tokenize input
    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, DELIMITERS);

        // concatenate tokens into entire_command
        if (token != NULL) {
            strcat(entire_command, " ");
            strcat(entire_command, token);
        }
    } 

    // check if the last token is "&" and exclude it
    size_t command_len = strlen(entire_command);
    if (strcmp(entire_command + command_len - 1, "&") == 0) {
        entire_command[command_len - 1] = '\0';
    }

    args[arg_count++] = NULL;

    // implement jobs command
    if (strcmp(args[0], "jobs") == 0){
        printJobs(job_list);
        return;
    }

    // implement kill command
    if (strcmp(args[0], "kill") == 0){
        if (args[1] == NULL){
            printf("kill: usage: kill [pid]");
            return;
        }
        int index = 1;
        if (strcmp(args[1], "-9") ==0){
            index = 2;
        }
        int num = atoi(args[index]);
        if (num == 0 && args[index][0] != '0') {
            printf("kill: %s: arguments must be process",args[index]);
            return;
        }
        if(index == 2){
            kill((pid_t)num,SIGKILL);
        }else{
            kill((pid_t)num,SIGTERM);
        }
        return;
    }

    // implement fg command
    if (strcmp(args[0], "fg") == 0) {
    struct Job* jobToResume = NULL;
        // if a jobid is provided
        if (args[1] != NULL) {
            int jobId;
            if (args[1][0]=='%'){
                jobId = atoi(args[1]+1);
                if (jobId == 0 && args[1][1] != '0') {
                    printf("Invalid job ID: %s\n",args[1]+1);
                    return;
                }
            }else{
                jobId = atoi(args[1]);
                if (jobId == 0 && args[1][0] != '0') {
                    printf("Invalid job ID: %s\n",args[1]);
                    return;
                }
            }

            for (struct Job* job = job_list; job != NULL; job = job->next) {
                if (job->jobId == jobId) {
                    jobToResume = job;
                    break;
                }
            }

            if (!jobToResume) {
                printf("Job [%d] not found.\n", jobId);
                return;
            }

            if (jobToResume->status != BLOCKED) {
                printf("Job [%d] is not suspended and cannot be resumed.\n", jobId);
                return;
            }
        }else {
            // if the jobid is not provided, find the last suspended job
            for (struct Job* job = tail; job != NULL; job = job->prev) {
                if (job->status == BLOCKED) {
                    jobToResume = job;
                    break;
                }
            }

            if (!jobToResume) {
                printf("No suspended job found to resume in the background.\n");
                return;
            }
        }

        pid_t fgPid = jobToResume->pid;

        // save current terminal settings
        struct termios savedSettings;
        if (tcgetattr(STDIN_FILENO, &savedSettings) == -1) {
            perror("tcgetattr");
            return;
        }

        // restore terminal settings
        if (tcsetattr(STDIN_FILENO, TCSADRAIN, &(jobToResume->setting)) == -1) {
            perror("tcsetattr");
            return;
        }

        // set terminal pgroup to fg pgroup
        if (tcsetpgrp(STDIN_FILENO, fgPid) == -1) {
            perror("tcsetpgrp");
            return;
        }

        // send SIGCONT signal if the job is stopped
        if (kill(-fgPid, SIGCONT) == -1) {
            perror("kill");
            return;
        }

        // wait for the foreground process to complete
        int status;
        if (waitpid(fgPid, &status, WUNTRACED) == -1) {
            perror("waitpid");
        }

        // set terminal pgroup back to the shell
        if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1) {
            perror("tcsetpgrp");
        }

        // restore original terminal settings
        if (tcsetattr(STDIN_FILENO, TCSADRAIN, &savedSettings) == -1) {
            perror("tcsetattr");
        }
        return;
    }

    // implement bg command
   if (strcmp(args[0],"bg") == 0) {
        struct Job* jobToResume = NULL;
        // if a jobid is provided
        if (args[1] != NULL) {
            int jobId;
            if (args[1][0]=='%'){
                jobId = atoi(args[1]+1);
                if (jobId == 0 && args[1][1] != '0') {
                    printf("Invalid job ID: %s\n",args[1]+1);
                    return;
                }
            }else{
                jobId = atoi(args[1]);
                if (jobId == 0 && args[1][0] != '0') {
                    printf("Invalid job ID: %s\n",args[1]);
                    return;
                }
            }
            for (struct Job* job = job_list; job != NULL; job = job->next) {
                if (job->jobId == jobId) {
                    jobToResume = job;
                    break;
                }
            }
            if (!jobToResume) {
                printf("Job [%d] not found.\n", jobId);
                return;
            }

            if (jobToResume->status != BLOCKED) {
                printf("Job [%d] is not suspended and cannot be resumed.\n", jobId);
                return;
            }
        }else {
        // if the jobid is not provided, find the last suspended job
            for (struct Job* job = tail; job != NULL; job = job->prev) {
                if (job->status == BLOCKED) {
                    jobToResume = job;
                    break;
                }
            }
            if (!jobToResume) {
                printf("No suspended job found to resume in the background.\n");
                return;
            }
        }
        if (kill(-(jobToResume->pid), SIGCONT) < 0) {
            perror("Error sending SIGCONT to job");
            return;
        }
        jobToResume->is_background = 1; // mark the job as running in the background
        printf("Resumed job [%d] %s in the background.\n", jobToResume->jobId, jobToResume->command);
        return;
     }

    // check if the command is ended with '&'
    size_t len = strlen(args[arg_count-2]);
    if (len > 0 && args[arg_count-2][len - 1] == '&') {
        run_in_background = 1;
        args[arg_count-2][len-1] = '\0';
        if(len==1){args[arg_count-2]='\0';}
    }

    // start a child process to execute command
    pid_t pid = fork();

    if (pid == 0) { // child

        // set ignored signals to default
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

        // search PATH and execute the command
        execvp(args[0], args);  
        perror("execvp fails"); 
        exit(EXIT_FAILURE);

    } else if (pid > 0){  // parent 
        addJob(run_in_background, entire_command, pid);

        if (setpgid(pid, 0) == -1) {
            perror("setpgid");
            exit(EXIT_FAILURE);
        }

        if (run_in_background){
            tcsetpgrp(STDIN_FILENO, getpgrp());
        }else{
            tcsetpgrp(STDIN_FILENO, pid);
            // wait for the child to terminate
            int status;
            waitpid(pid, &status, WUNTRACED); 
            tcsetpgrp(STDIN_FILENO, getpgrp());
        }
         
    } else { // bad fork
        perror("fork fails");
        exit(EXIT_FAILURE);
    }
}

void child_handler(int signum, siginfo_t *info, void *context){

    pid_t pid = info -> si_pid; 
    //si_code in the info struct contains if the child is suspended, exited, or interrupted...
 	
   	if(info->si_code == CLD_EXITED){
        printf("Exited\n");
        int status;
        waitpid(pid, &status, WNOHANG);
        add(&revise_list,pid,EXIT_FLAG);
    }
    if(info->si_code== CLD_KILLED){
        // avoid zombie process
        int status;
        printf("Killed\n");
        waitpid(pid, &status, WNOHANG);
        add(&revise_list,pid,STOP_FLAG);
    }
    if(info->si_code == CLD_CONTINUED){
        printf("Resumed\n");
        struct termios setting;
        add(&revise_list,pid,SUSPEND_FLAG);
	}
    if(info->si_code == CLD_STOPPED){
        kill(pid, SIGSTOP);
        printf("Suspended\n");
        add(&revise_list,pid,SUSPEND_FLAG);
   	}
}

void updateJob(){

	while(revise_list !=NULL){
		pid_t pid = revise_list->pid ;
		int flag = revise_list->flag;
		if (flag == EXIT_FLAG || flag == STOP_FLAG){
            removeJob(pid);
            return;
        }

        struct Job* current = job_list;
        while (current != NULL && current->pid != pid) {
            current = current->next;
        }

        if (current == NULL) {
            printf("Job with PID %d not found\n", pid);
            return;
        }
        if(flag == SUSPEND_FLAG){
            current->status = BLOCKED;
            current->setting = revise_list->setting;
        }
        if(flag == RESUME_FLAG){
            current->status = RUNNING;
        }
        struct Revise* head = pop(&revise_list);
        free(head);
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
    
    while (1) {
        // reset or empty input each time
        if (input){
            free(input);
            input = (char *)NULL;
        }

        // get command
        input = readline("mysh> ");
        
        if (!input){
            printf("\n");
            break;
        }
        
        updateJob();
        if (*input){
            execute_command(input);
        }
    }
    
    // cleanup 
    free(input);
    input = (char *)NULL;
    freeList(revise_list);
    freeJobList();
    return 0;
}