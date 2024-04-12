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
    
    while true: 
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
    
    
    // cleanup - free input and joblist
    
    return 0;
}
