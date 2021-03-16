#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>

// Node Structure for Storing Commands
struct Node{
    char** arguments; // to store the array of arguments
    int argSize; // to store the number of arguments 
    pid_t pid; // stores the pid of the process
    int running;
    struct Node* next; // to store the next pointer
    struct Node* prev; // to store the previous pointer
};
// Declare global pointers for storing linked list
struct Node* topCurrent= NULL;
struct Node* top = NULL;
struct Node* tail = NULL;


void push(char** arguments, int argSize, pid_t pid){

    /*Inserts a new command in the Fullhistory Doubly Linked List
      inputs-> 1. arguments string list
               2. argument list size
               3. Pid of the process.
    */

    struct Node* current = top;
    // allocate new memory for the new command
    struct Node* newNode = malloc(sizeof(struct Node));
    //printf("pushing %s in stack\n", arguments[0]);
    
    newNode->argSize = argSize;
    newNode->pid = pid;
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
        tail = newNode;
    }
    else{
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

void pushCurrent(char** arguments, int argSize, pid_t pid)
{
    /*Inserts a new command in the Current History Linked List
      inputs-> 1. arguments string list
               2. argument list size
               3. Pid of the process.
    */
    struct Node* current = topCurrent;
    // allocate new memory for the new command
    struct Node* newNode = malloc(sizeof(struct Node));
    
    newNode->argSize = argSize;
    newNode->pid = pid;
    newNode->running = 1; // set running to True
    newNode->arguments = (char **) malloc(sizeof(char *)*(argSize+1));
    
    // Copy each and every command
    for(int i=0;i<argSize;i++){
        newNode->arguments[i] = (char *)malloc(sizeof(char)*sizeof(strlen(arguments[i])+1));
        strcpy(newNode->arguments[i], arguments[i]);
    }
    // find the end of the linked list and insert the newNode
    newNode->next = NULL;
    if(current == NULL){
        topCurrent = newNode;
    }
    else{
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
    }
}

void deleteNode(pid_t pid)
{
    /* Deletes a node from the Current Linked List with given pid value
       Args-> pid
    */

    struct Node *currNode = topCurrent;
    struct Node* prev = NULL;
    // if head is deleted
    if(currNode!=NULL && currNode->pid == pid)
    {
        topCurrent = topCurrent->next;
        free(currNode);
        return;
    }
    // not head
    else{
        while(currNode != NULL && currNode->pid != pid)
        {
            prev = currNode;
            currNode = currNode->next;
        }
        if(currNode == NULL) return;
        prev->next = currNode->next;
        free(currNode);
    }
}


struct Node* searchCommand(char* command)
{
    /*Search the command based on the first argument
      Args-> command string.
      Returns -> returns pointer to Node having the command having the first argument
                 as command, if no such Node exists NULL is returned. 
    */
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
     Returns -> pointer to a Node or NULL if no command found 
     */
    struct Node* currNode = tail;
    int count = 1;
    while(currNode != NULL && count!=x)
    {
        currNode = currNode->prev;
        count++;
    }
    return currNode;
}

void displayFull(int numberOfCommandsINT){
    /* Traverse each Node and print all the arguments
       Args-> numberOfCommandsINT : specifying the number of commands to be printed
    */

    if(tail == NULL){
        printf("NO history commands!!\n");
    }else{
        struct Node* currNode = tail;
        int commandNumber = 1;
        while(currNode != NULL && commandNumber<=numberOfCommandsINT){
            printf("%d. ",commandNumber);
            for(int i=0;i<currNode->argSize;i++){
                printf("%s ", (currNode->arguments)[i]);
            }
            printf("\n");
            currNode = currNode->prev;
            commandNumber++;
        }
    }
}

void displayBrief(){
    /* 
       Display history in brief format.
       Traverse each node and print the first element of the array argument.
    */
    if(top == NULL){
        printf("NO processes spawned till now!!\n");
    }else{
        struct Node* currNode = top;
        int commandNumber = 1;
        while(currNode != NULL){
            printf("%d.command name:%s    ",commandNumber, (currNode->arguments)[0]);
            printf("process id:%d\n",currNode->pid);
            currNode = currNode->next;
            commandNumber++;
        }
    }
}

void displayCurrent()
{
    /*
    Display all current processes by traversing each Node of current process
    */ 

   struct Node* currNode = topCurrent;
   int commandNumber = 1;
    while(currNode != NULL){
        // print the process if it is currently running
        if(currNode->running == 1){
        printf("%d.command name: ",commandNumber);
        printf("%s    ", (currNode->arguments)[0]);
        printf("process id: %d\n", currNode->pid);
        commandNumber++;
        }
        currNode = currNode->next;   
    }
    if(commandNumber == 1)
    {
        // if no current processes are running 
        printf("No current processess running\n");
    }
    return;
}


void printCommand(char** arguments){
    /*
    Helper function to print the command in specific format
    Args-> array of arguments
    */

    printf("Executing command \"");
    int i = 0;
    while((arguments)[i+1] != NULL)
    {
        printf("%s ", (arguments)[i]);
        i++;
    }
    printf("%s\"\n", (arguments)[i]);
}

int parseIORedirection(char** args, char** inputStream, char** outputStream)
{
    /* 
        Function to set input and outputStreams to the variable defined
       1. Also look for parse errors for multiple <, > provided.
       Args-> array of arguments, inputStream, outputStream
       Returns-> 0 : on successfull parse.
                -1: on error parsing.
    */

    int i=0;
    while(args[i]!=NULL)
    {
        if(strcmp(args[i],"<") == 0)
        {
            // Input Stream symbol
            if(args[i+1]!=NULL)
            {
                *inputStream = malloc((strlen(args[i+1])+1)*sizeof(char));
                strcpy(*inputStream, args[i+1]);
            }else{
                printf("Parse error at \"<\" \n");
                return -1;
            }
            for(int j=i;args[j+1]!=NULL;j++)
            {
                // shift every arg to get new string.
                args[j] = args[j+2];
            }
        }
        else if(strcmp(args[i], ">") == 0)
        {
            // Output Stream symbol
            if(args[i+1]!=NULL)
            {
                *outputStream = malloc((strlen(args[i+1])+1)*sizeof(char));
                strcpy(*outputStream, args[i+1]);
            }else{
                printf("Parse error at \">\" \n");
                return -1;
            }
            for(int j=i;args[j+1]!=NULL;j++)
            {
                // shift every arg to get new string.
                args[j] = args[j+2];
            }
        }
        else{
            // update i only if not found
            i++;
        }
    }
    return 0;
}

int cntPipes(char** args)
{
    /*
     Counts the number of | tokens in arguments array.
     Args-> array of arguments
    */
    int cnt=0;
    for(int i=0;args[i]!=NULL;i++)
    {
        if(strcmp(args[i],"|")==0) cnt++;
    }
    return cnt;
}

int findNextPipe(int start, char** args)
{
    /*
        Finds the next Index of | token.
        Args-> start index, array of arguments
        Returns -> Index of next position if found
                else return -1
    */
    for(int i=start+1;args[i]!=NULL;i++){
        if(strcmp(args[i],"|")==0) return i;
    }
    return -1;
}

void executeCommand(char** args, int background, int argSize, int in1, int o1)
{
    /*
     Helper function to execute Command using execVP.
     Args-> array of arguments, background (T/F), argument Size, 
            input stream(FD), output stream(FD).
    */

    pid_t  pid;
    int    status;
    pid = fork();
    if (pid == 0) {
        // If Child Process
        if(background == 1){
            // set a new process group id to run process in Background
            int pgidResult = setpgid(0,0);
            if(pgidResult == -1){
                printf("** Background process spawned failed!! **\n");
                exit(1);
            }
        }
        // If Input File Descriptor is not zero, connect it to Pipe
        if(in1 != 0)
        {
            dup2(in1,0);
            close(in1);
        }
        // If Output File Descriptor is not 1, connect it to Pipe.
        if(o1 != 1)
        {
            dup2(o1,1);
            close(o1);
        }
        char* inputStream = NULL;
        char* outputStream = NULL;
        int pStatus = parseIORedirection(args, &inputStream, &outputStream);
        if(pStatus == -1)
        {
            // if parsing is failed
            exit(1);
        }
        //printf("I:%s, O:%s\n", inputStream, outputStream);
        if(inputStream != NULL){
            int fd0;
            if ((fd0 = open(inputStream, O_RDONLY, 0)) < 0) {
                perror("Couldn't open input file");
                exit(1);
            }           
            // dup2() copies content of fdo in input of preceeding file
            dup2(fd0, STDIN_FILENO); 

            close(fd0); 
        }
        if(outputStream != NULL)
        {
            int fd1 ;
            if ((fd1 = creat(outputStream , 0644)) < 0) {
                perror("Couldn't open the output file");
                exit(1);
            }           

            dup2(fd1, STDOUT_FILENO); 
            close(fd1);
        }
        int status = execvp(args[0], args);
        if(status < 0)
        {
            // Exec command failed
            printf("** Invalid command **\n");
            exit(1);
        }
    }
    else if(pid == -1){
        // fork failed
       printf("** Fork failed **\n");
       exit(1);
    }
    else{
        // parent process
        if(argSize!=0) push(args,argSize,pid);
        if(background!=1) waitpid(pid,&status,0);
        else{
            if(argSize!=0) pushCurrent(args,argSize,pid);
            printf("Process running with pid %d running in background\n", pid);
        }
    }
}


void executePipeCommands(char** args,int background,int argSize)
{
    int numPipes = cntPipes(args);
    int inputStream, en, p[2];
    inputStream = STDIN_FILENO;
    int st=-1;
    for(int i=0; i<numPipes; i++)
    {
        pipe(p);
        en = findNextPipe(st,args);
        char* temp = args[en];
        args[en] = NULL;
        executeCommand(args+st+1,background,en-st-1,inputStream,p[1]);
        close(p[1]);
        args[en] = temp;
        inputStream = p[0];
        st = en;
    }
    executeCommand(args+st+1,background,argSize-st-1,inputStream,STDOUT_FILENO);
}

// helper function to process command and split into arguments
void processCommand(char* command, char** arguments, int* i)
{
    /* Args -> command string, a pointer to an array of arguments 
       Stores each args in the argument array terminated by NULL character */
    int j = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL)
    {
        arguments[j] = arg;
        arg = strtok(NULL, " ");
        j++;
    }
    arguments[j] = NULL;
    *i = j;
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

void removeKilledProcess()
{
    /* Function routine to remove all killed processes from Current List
       for freeing up memory */
    struct Node* currNode = topCurrent;
    while(currNode != NULL)
    {
        if(currNode->running==0)
        {
            deleteNode(currNode->pid);
        }
        currNode = currNode->next;
    }
    return;
}

void handle_childkills(int sig){
    /* Function to handle SIGCHLD signals, 
       Checks status of for each process and sets the running status to 0*/
    //printf("Caught SigChld\n");
    pid_t pid1;
    int status;
    struct Node* currNode = topCurrent;
    while (currNode!=NULL)
    {
        pid1 = waitpid(currNode->pid,&status,WNOHANG);
        if(pid1 == -1)
        {
            printf("\nChild process with pid %d was terminated\n", currNode->pid);
            currNode->running = 0;
        }else if(pid1!=0 && (WIFEXITED(status) || WIFSIGNALED(status) || WCOREDUMP(status))){
            printf("\nChild process with pid %d was terminated\n", currNode->pid);
            currNode->running = 0;
        }
        currNode = currNode->next;
    }
    
    return;
}
size_t size = 1024;

void replaceSubstring(char* string,char* sub,char* new_str)
{
    // replace substring for changing absolute path
    int stringLen,subLen,newLen;
    int i=0,j,k;
    int flag=0,start,end;
    stringLen=strlen(string);
    subLen=strlen(sub);
    newLen=strlen(new_str);

    for(i=0;i<stringLen;i++)
    {
        flag=0;
        start=i;
        for(j=0;string[i]==sub[j];j++,i++)
            if(j==subLen-1) flag=1;
            end=i;
            if(flag==0) i-=j;
            else
            {
                for(j=start;j<end;j++)
                {
                    for(k=start;k<stringLen;k++) string[k]=string[k+1];
                    stringLen--;
                    i--;
                }

                for(j=start;j<start+newLen;j++)
                {
                    for(k=stringLen;k>=j;k--) string[k+1]=string[k];
                    string[j]=new_str[j-start];
                    stringLen++;
                    i++;
                }
            }
    }
}

// updatePath the path on calling cd
void updatePath(char* pwd, char* homeDir){
    pwd = getcwd(pwd, size);
    replaceSubstring(pwd,homeDir,"~");
}

// to remove tabs from input buffer
void replaceTabs(char* buf)
{
    int n = strlen(buf);
    for(int i=0;i<n;i++)
    {
        if(buf[i]=='\t' || buf[i] == '\r') buf[i]=' ';
    }
}

// strip the spaces at the end of the file
void rstrip(char* buf)
{
    int n = strlen(buf);
    int i;
    for(i=(n-1); i>=0; i--)
    {
        if(buf[i]==' ' || buf[i]=='\n') continue;
        else break;
    }
    buf[i+2] = '\0';
}

int executeCommandLine(char* buf, char* pwd, char* homeDir, char* procName){
    /* Execute the command entered in command line.
       1. Clean the command from unwanted spaces and replace Tabs.
       2. Compare the tokens to execute the appr*/
    replaceTabs(buf);
    rstrip(buf);
    if(strcmp(buf,"\n")==0)
    {
        // handle enters
        return 1;
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
        if(option == NULL){
            return 1;
        }
        // stop command
        if(strcmp(option,"STOP") == 0){
            return -1;
        }
        // history command
        else if(strcmp(option,"HISTORY") == 0){
            //printf("Inside history option\n");
            //if no args provided
            if(subOption == NULL){
                printf("Provide arguments FULL/BRIEF to command\n");
                return 0;
            }
            // for full history
            if(strcmp(subOption,"FULL") == 0)
            {
                //"Execute full option"
                displayFull(10);
            }
            // for brief history
            else if(strcmp(subOption,"BRIEF") == 0)
            {
                //"Execute brief history"
                displayBrief();
            }else{
                printf("Wrong arguments provided!!\n");
                return 0;
            }
        }
        else if(strcmp("pid",option) == 0){
            // handle pid option
            char** arguments = (char **)malloc(sizeof(char *)*strlen(bufCopy));
            int i;
            processCommand(bufCopy, arguments, &i);
            push(arguments, i, getpid());

            free(arguments);
            if(subOption == NULL)
            {
                // print pid of current process
                printf("command name: %s    process id: %d\n", procName, getpid());
                return 0;
            }else if(strcmp("current", subOption) == 0)
            {
                // print all current process
                displayCurrent();
                return 0;
            }else if(strcmp("all", subOption) == 0)
            {
                // print all processes
                displayBrief();
                return 0;
            }else{
                printf("Wrong arguments provided!!\n");
                return 0;
            }
        }
        else if(strcmp("cd",option) == 0){
            // handle cd command
            char** arguments = (char **)malloc(sizeof(char *)*strlen(bufCopy));
            int i;
            processCommand(bufCopy, arguments, &i);
            push(arguments, i, getpid());

            free(arguments);
            int dirResult;
            // overwriting the ~ home directory command
            if(strcmp("~", subOption) == 0){
                dirResult = chdir(homeDir);
            }else dirResult = chdir(subOption);
            // if path not found
            if(dirResult<0){
                perror("Cannot direct to path specified");
                return 0;
            }else{
                updatePath(pwd, homeDir);
                printf("Changed working directory to %s\n", pwd);
            }
        }
        else if(strncmp("HIST",option,4) == 0)
        {
            // print last n commands
            char* numberOfCommands = (char *) malloc(sizeof(char)*(strlen(option)-3));
            strcpy(numberOfCommands,bufCopy+4);
            int numberOfCommandsINT = atoi(numberOfCommands);
            if(numberOfCommandsINT == 0 && (strcmp("0",numberOfCommands) == 0)) return 0;
            else if(numberOfCommandsINT == 0)
            {
                // error in providing number of commands
                printf("Please provide command in correct format \"HISTN\" where \"N\" should be an integer\n");
                return 0;
            }else{
                // non zero number of commands
                displayFull(numberOfCommandsINT);
                return 0;
            }
        }
        else if(strncmp("!HIST",option,5) == 0){
            // execute command with index number
            char* numberOfCommands = (char *) malloc(sizeof(char)*(strlen(option)-3));
            strcpy(numberOfCommands,bufCopy+5);
            int numberOfCommandsINT = atoi(numberOfCommands);
            if(numberOfCommandsINT == 0)
            {
                // wrong argument
                printf("Please specify correct number from 1-N\n");
                return 0;
            }else{
                struct Node* command = countCommand(numberOfCommandsINT);
                if(command == NULL)
                {
                    // command not found
                    printf("No command on the given index\n");
                    return 0;
                }else{
                    // command found, first reconstruct the command and recall the function
                    int totSize = 0;
                    for(int i=0;i<command->argSize;i++)
                    {
                        totSize += strlen(command->arguments[i]);
                        totSize += strlen(" ");
                    }
                    char* fullCommand = (char *)malloc(sizeof(char)*(totSize+1));

                    for(int i=0;i<command->argSize;i++)
                    {
                        strcat(fullCommand,command->arguments[i]);
                        strcat(fullCommand," ");
                    }
                    return executeCommandLine(fullCommand, pwd, homeDir, procName);
                }
            }
        }
        else{
            // a shell command is executed
            int background;
            if(bufCopy[n-2]=='&')
            {
                // if background process
                background = 1;
                bufCopy[n-2]='\0';
            }else{
                background = 0;
            }
            char** arguments = (char **)malloc(sizeof(char *)*strlen(bufCopy));
            int i;
            processCommand(bufCopy, arguments, &i);
            executePipeCommands(arguments, background, i);

            free(arguments);
        }
        printf("\n");
        free(buf); // free the buffer
        free(bufCopy); // free the copy of buffer allocated
        return 0;
}
// function to kill all background processes
void killAllCurrent()
{
    struct Node* currNode = topCurrent;
    if(currNode != NULL){
        printf("Some Processes are running\n");
        printf("Killing currently running background processes!!\n");
    }
    while(currNode != NULL)
    {
        kill(currNode->pid, SIGKILL);
        currNode = currNode->next;
    }
}


int main(int argc, char* argv[])
{
    printf("\n");
    printf("**************************************************************\n\n");
    printf("CSL331 Bash terminal © Samarth Singh\n\n");
    printf("For using >, < or | use spaces between them otherwise invalid command will be prompted\n\n");
    printf("**************************************************************\n\n");
    // setup a signal handler
    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_childkills);
    // declare variables
    char* hostName = (char *)malloc(sizeof(char)*size);
    char* userName = (char *)malloc(sizeof(char)*size);
    char* defaultHostName = "DefaultHostName";
    char* defaultUserName = "DefaultUserName";
    char* homeDir = (char *)malloc(sizeof(char)*size);

    // store current homeDir 
    homeDir = getcwd(homeDir, size);
    char* pwd = (char *)malloc(sizeof(char)*size);
    int result;
    // get hostName
    result = gethostname(hostName,size);

    if(result<0)
    {   
        // if hostName cant be fetched
        strcpy(hostName, defaultHostName);
    }

    result = getlogin_r(userName, size);
    if(result<0)
    {
        // if userName cant be fetched
        strcpy(userName, defaultUserName);
    }
    updatePath(pwd, homeDir);
    while(1){
        removeKilledProcess();
        printf("<%s@%s:%s>",userName,hostName,pwd);
        // again allocate a memory buffer
        char* buf;
        buf = (char *)malloc(sizeof(char)*1024);
        int endofFile = getline(&buf, &size, stdin); 
        if(endofFile == -1)
        {
            // handling ctrl D
            printf("\n\nAborting forcefully!!\n\n");
            killAllCurrent();
            exit(1);
        }
        // execute the command 
        int exitStatus = executeCommandLine(buf,pwd,homeDir,argv[0]);
        if(exitStatus == -1) break;
    }
    // free the history
    freeHistory();
    printf("Exiting normally,bye!\n");
    // kill all current processes
    killAllCurrent();
    return 0;
}