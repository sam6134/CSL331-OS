#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
    printf("Task-2 Getting positions set by child to zero from parent using Exec command\n");
    printf("Child process compiled using make to make a execultable 0065-task2Child\n\n");
    pid_t pid;
    int r;
    int a;
    int b;
      // check a and b
    FILE *fp;
    fp = fopen("test.txt", "w+");
    // write 1 to 100
    for(int i=1;i<=100;i++)
    {
        putw(i, fp);
        fputc(' ',fp);
    }
    fclose(fp);
    pid=fork();

    if(pid==0)
    { //CHILD PROCESS
            
            char *args[]={"./0065-task2Child",NULL}; 
            execvp(args[0],args); 
            //printf("Child : %d, %d\n",pid, getpid());
	        exit(0);
    }
    else
    {
         //PARENT PROCESS
            wait(NULL);
            fp = fopen("test.txt", "r");
            printf("\nRandom integers from parents are-\n");
            for(int i=0;i<100;i++){
                int x = getw(fp);
                char dummy = getc(fp);
                if(x == 0)
                {
                    printf("%d ",i);
                }
            }
            printf("\n");
            //printf("\nParent %d, %d\n",pid, getpid());
            fclose(fp);
    }
    return 0;
}