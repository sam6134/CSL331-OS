#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>

// Node Structure for Storing Commands
struct Node{
    char** arguments; // to store the array of arguments
    int argSize; // to store the number of arguments 
    struct Node* next; // to store the next pointer
};


struct Node* top = NULL;

// Function to Insert a new command in the histoty
void push(char** arguments, int argSize){
    struct Node* current = top;
    // allocate new memory for the new command
    struct Node* newNode = malloc(sizeof(struct Node));
    //printf("pushing %s in stack\n", arguments[0]);
    
    newNode->argSize = argSize;
    newNode->arguments = (char **) malloc(sizeof(char *)*(argSize+1));
    //printf("argSize is %d\n", argSize);
    // Copy each and every command
    for(int i=0;i<argSize;i++){
        newNode->arguments[i] = (char *)malloc(sizeof(char)*sizeof(strlen(arguments[i])+1));
        strcpy(newNode->arguments[i], arguments[i]);
    }
    // find the end of the linked list and insert the newNode
    newNode->next = NULL;
    if(current == NULL){
        top = newNode;
    }
    else{
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
    }
}

// to search the command based on the first argument
struct Node* searchCommand(char* command)
{
    struct Node* currNode = top;
    while(currNode != NULL && (strcmp((currNode->arguments)[0], command) != 0))
    {
        currNode = currNode->next;
    }
    return currNode;
}

// to find the command based on the index Number
struct Node* countCommand(int x)
{
    /* Args -> Index
     Returns -> pointer to a Node or NULL if no command found */
    struct Node* currNode = top;
    int count = 1;
    while(currNode != NULL && count!=x)
    {
        currNode = currNode->next;
        count++;
    }
    return currNode;
}

// displays the full history
void displayFull(){
    /* Traverse each Node and print all the arguments*/
    if(top == NULL){
        printf("NO history commands!!\n");
    }else{
        struct Node* currNode = top;
        int commandNumber = 1;
        while(currNode != NULL){
            printf("%d. ",commandNumber);
            for(int i=0;i<currNode->argSize;i++){
                printf("%s ", (currNode->arguments)[i]);
            }
            printf("\n");
            currNode = currNode->next;
            commandNumber++;
        }
    }
}

// displays the history commands in brief format
void displayBrief(){
    /* traverse each node and print the first element of the array argument*/
    
    if(top == NULL){
        printf("NO history commands!!\n");
    }else{
        struct Node* currNode = top;
        int commandNumber = 1;
        while(currNode != NULL){
            printf("%d. %s\n",commandNumber, (currNode->arguments)[0]);
            currNode = currNode->next;
            commandNumber++;
        }
    }
}

// helper function to print the command in specific format
void printCommand(char** arguments){
    printf("Executing command \"");
    int i = 0;
    while((arguments)[i+1] != NULL)
    {
        printf("%s ", (arguments)[i]);
        i++;
    }
    printf("%s\"\n", (arguments)[i]);
}

// helper function to execute Command using exec VP
void executeCommand(char** args)
{
    pid_t  pid;
    int    status;
    pid = fork();
    if (pid == 0) {
        // If Child Process
        int status = execvp(args[0], args);
        if(status < 0)
        {
            // Exec command failed
            printf("** Invalid command **\n");
            exit(1);
        }
    }
    else {
        // Parent Process
        wait(NULL);
    }
}

// helper function to process command and split into arguments
void processCommand(char* command, char** arguments)
{
    /* Args -> command string, a pointer to an array of arguments 
       Stores each args in the argument array terminated by NULL character */
    int i = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL)
    {
        arguments[i] = arg;
        arg = strtok(NULL, " ");
        i++;
    }
    arguments[i] = NULL;
}

// helper function to free history LinkList
void freeHistory()
{
    // traverse each Node 
    printf("Clearing History Memory....\n");
    struct Node* currNode = top;
    struct Node* nextNode = top;
    while(currNode != NULL)
    {   
        nextNode = currNode->next;
        for(int i=0;i<currNode->argSize;i++){
            // free memory for each argument
            free((currNode->arguments)[i]);
        }
        // free the argument pointer
        free(currNode->arguments);
        // free the Node
        free(currNode);
        currNode = nextNode;
    }
    free(nextNode);
    printf("Memory Cleared Successfully!!\n");
}

// for handling Ctrl-C events
void handle_sigint(int sig) 
{ 
    printf("Process Interupted!!\n"); 
} 

int main(int argc, char* argv[])
{
    // setup a signal handler
    signal(SIGINT, handle_sigint);
    // Traverse all the files
    for(int fileIdx = 1; fileIdx < argc; fileIdx++){
        // Open the file
        FILE *fp;
        fp = fopen(argv[fileIdx], "r");
        if(fp == NULL){
            printf("Fail to read the file \"%s\". Skipping the current file.\n",argv[fileIdx]);
            continue;
        }
        // allocate buffer
        char* buf;
        size_t size = 1024;
        buf = (char *)malloc(sizeof(char)*size);
        int cnt=0;
        // Using getline to allocate space according to the command length in the file
        while(getline(&buf,&size,fp)!= -1)
        {
            printf("Command to be executed - %s\n", buf);
            int n = strlen(buf);
            //printf("n=%d\n", n);
            // If last character is newLine overite with EOF
            if(buf[n-1] == '\n'){
            buf[n-1] = '\0';
            }
            // declare an array of arguments
            char** arguments = (char **)malloc(sizeof(char *)*n);
            int i = 0;
            // use the tokenizer to split arguments on a space delim
            char *arg = strtok(buf, " ");
            while (arg != NULL)
            {
                arguments[i] = arg;
                arg = strtok(NULL, " ");
                i++;
            }
            arguments[i] = NULL;
            //printf("%d\n", i);
            // push the arguments in the history and execute
            push(arguments, i);
            executeCommand(arguments);
            // free the arguments memory
            free(arguments);
        }
        // free the buffer allocated
        free(buf);
        
    }
    //displayBrief();
    printf("Post-batch processing interactive mode:\n");
    while(1){
        printf("Please enter your command:\n\n");
        // again allocate a memory buffer
        char* buf;
        size_t size = 1024;
        buf = (char *)malloc(sizeof(char)*1024);
        getline(&buf, &size, stdin); 
        if(strcmp(buf,"\n")==0)
        {
            continue;
        }
        int n = strlen(buf);
        // replace the last character by EOF
        buf[n-1]='\0';
        char* bufCopy = malloc((n+1)*sizeof(char));
        // make a copy of command to avoid mutation via strtok
        strcpy(bufCopy, buf);
        // split the first two tokens to determine the command
        char* option = strtok(buf," ");
        char* subOption = strtok(NULL," ");
        // stop command
        if(strcmp(option,"STOP") == 0){
            break;
        }
        // history command
        else if(strcmp(option,"HISTORY") == 0){
            //printf("Inside history option\n");
            //if no args provided
            if(subOption == NULL){
                printf("Provide arguments FULL/BRIEF to command\n");
                continue;
            }
            // for full history
            if(strcmp(subOption,"FULL") == 0)
            {
                //"Execute full option"
                displayFull();
            }
            // for brief history
            else if(strcmp(subOption,"BRIEF") == 0)
            {
                //"Execute brief history"
                displayBrief();
            }else{
                printf("Wrong arguments provided!!\n");
                continue;
            }
        }
        // exec option
        else if(strcmp(option, "EXEC") == 0)
        {
            //"Execute exec\n"
            if(subOption == NULL)
            {
                printf("Please provide an argument!\n");
                continue;
            }
            // converting the second option to an integer to determine the type of exec command called
            int x = atoi(subOption);
            if(x != 0)
            {
                // if exec called via command number
                // find the command
                struct Node* command = countCommand(x);
                if(command == NULL){
                    // no command found
                    printf("Invalid index number\n");
                    continue;
                }else{
                    // command found
                    printCommand(command->arguments);
                    executeCommand(command->arguments);
                    push(command->arguments, command->argSize);
                }
            }else{
                // if exec called via command name
                struct Node* command = searchCommand(subOption);
                if(command == NULL)
                {
                    // command not found
                    printf("No command found with specified name in History\n");
                    continue;
                }else{
                    // command found
                    printCommand(command->arguments);
                    executeCommand(command->arguments);
                    push(command->arguments, command->argSize);
                }

            }
        }
        
        else{
            // wrong option
            printf("Wrong command! Please try again!!\n");
            continue;
        }
        printf("\n");
        free(buf); // free the buffer
        free(bufCopy); // free the copy of buffer allocated
    }
    // free the history
    freeHistory();
    printf("Exiting normally,bye!\n");
    return 0;
}