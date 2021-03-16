#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

int main(int argc, char* argv[]){
    if(argv[1] && argv[2])
    {
        int n = atoi(argv[1]);
        int k = atoi(argv[2]);
        printf("n = %d,k= %d \n",n,k);
        for(int i=0;i<n;i++){
           for(int j =0; j<k; j++){
               // get ascii value of random character
               int randChar = 65+(rand()%57);
               char toPrint = (char) randChar;
               printf("%c",toPrint);
           }
           printf("\n");
        }
    }else{
        printf("Please specify the integer N and size k\n");
    }
}