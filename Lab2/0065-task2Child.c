#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
    FILE * fp;
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
}