#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>

// Node Structure for Storing Commands
struct PTE{
    unsigned int PFN: 14;
    unsigned int valid : 1;
    unsigned int present : 1;
    unsigned int protected : 3;
    unsigned int dirty : 1;
    unsigned int mode : 1;
};


// Lagged Fibonacci generator for random generation of virtual address spaces
void genLagFib(int n)
{
    uint32_t* arr = (uint32_t*) malloc(max(1000,n)*sizeof(uint32_t));
    int seed = 0x13;
    int p = 17;
    int q = 32;
    uint32_t m = (1<<16);
    for(int i=0;i<q;i++){
        arr[i] = seed;
    }

    // burning off 1000 numbers
    for(int i=q;i<1000;i++){
        arr[i] = (arr[i-q]%m + arr[i-p]%m)%m;
    }
    // initializing the new terms
    for(int i=0;i<q;i++){
        arr[i] = arr[999-i];
    }
    // again calculating the terms
    for(int i=q;i<n;i++){
        arr[i] = (arr[i-q]%m + arr[i-p]%m)%m;
    }

    for(int i=0;i<n;i++){
        printf("%d\n", arr[i]);
    }
}

int main(int argc, char* argv[])
{
    printf("\n");
    printf("**************************************************************\n\n");
    printf("CSL331 Simulation-1 © Samarth Singh\n\n");
    printf("**************************************************************\n\n");
    

    
    
    return 0;
}