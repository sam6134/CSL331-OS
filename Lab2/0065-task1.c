#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
    printf("Task-1 Getting positions set by child to zero from parent using fork() command\n\n");
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
            
            fp = fopen("test.txt", "r+");
            fseek(fp,0,SEEK_SET);
            printf("Random integers set by child are-\n");
            
            for(int i=0;i<10;i++){
                int randIdx = rand()%100;
                printf("%d ", randIdx);
                fseek(fp,(sizeof(int)+sizeof(char))*randIdx,SEEK_SET);
                putw(0, fp);
            }
            fclose(fp);
            printf("\nChild : fork id - %d and pid is %d\n",pid, getpid());
	        exit(0);
    }
    else
    {
         //PARENT PROCESS
            wait(NULL);
            fp = fopen("test.txt", "r");
            printf("Random integers from parents are-\n");
            for(int i=0;i<100;i++){
                int x = getw(fp);
                char dummy = getc(fp);
                if(x == 0)
                {
                    printf("%d ",i);
                }
            }
            printf("\nParent fork id -%d and pid is %d\n",pid, getpid());
            fclose(fp);
    }
    return 0;
}