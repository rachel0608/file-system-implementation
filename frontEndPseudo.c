int main() {

    char *input = (char *)NULL;

    // ignore useless signals and init sigaction for child signals
    
    main loop: 
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
