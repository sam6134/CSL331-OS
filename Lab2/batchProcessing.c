#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    FILE *fp;
    fp = fopen(argv[1], "r");
    char buf[1024];
    int cnt=0;
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
      printf("Command - %s\n", buf);
      int n = strlen(buf);
      if(buf[n-1] == '\n'){
      buf[n-1] = '\0';
      }
      char *arguments[64];
      int i = 0;
      char *arg = strtok(buf, " ");
      while (arg != NULL)
      {
        arguments[i] = arg;
        arg = strtok(NULL, " ");
        i++;
      }
      arguments[i] = NULL;
    pid_t  pid;
    int    status;
    pid = fork();
    if (pid == 0) {
        int status = execvp(arguments[0], arguments);
        if(status < 0)
        {
          printf("** Error in exec **");
          exit(1);
        }
    }
    else {
         wait(NULL);
    } 
    }
    return 0;
}