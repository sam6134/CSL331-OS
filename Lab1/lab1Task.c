#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

void genLFSR(int n)
{
   uint16_t startState = 0xACEu;
   uint16_t newBit;
    for(int i=0;i<n;i+=1)
    {
        newBit = ((startState>>0)^(startState>>2)^(startState>>3)^(startState>>5));
        startState = (startState>>1) | (newBit<<15);
        printf("%d\n", startState);
    }
}

void genLCG(int n)
{
    uint32_t startState = 0xACEu;
    uint32_t a = 13;
    uint32_t c = 1;
    uint32_t m = (1<<16);
    for(int i=0;i<n;i++){
        startState = (a*startState + c)%m;
        printf("%d\n", startState);
    }
    printf("\n");
}
int max(int a,int b)
{
    if(a>b) return a;
    return b;
}
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

int main(int argc, char* argv[]){
    // printf("Enter option for random generator:\n");
    // printf("1. LFSR\n");
    // printf("2. LCG Method\n");
    // printf("3. Lagged Fibonacci\n");
    // int option;
    // scanf("%d",&option);
    // printf("Enter N:\n");
    // int n;
    // scanf("%d",&n);
    int n = 10000;
    int option = 1;
    printf("%d\n", n);
    if(option == 1){
        genLFSR(n);
    }else if(option == 2){
        genLCG(n);
    }else if(option == 3){
        genLagFib(n);
    }


}