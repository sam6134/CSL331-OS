#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

#define MAX 10000
pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int index_write = 0;
int arr[MAX];
 typedef struct __myarg_t {
    int a;
 } myarg_t;

 void * mythread(void * arg) {
    // myarg_t * m = (myarg_t * ) arg;
    int to_write;
    pthread_mutex_lock(&lock);
    to_write = index_write;
    index_write++;
    pthread_mutex_unlock(&lock);
    // printf("Thread is writing %d\n", m->a);
    arr[to_write] = rand();
    return NULL;
 }

int main(int argc, char * argv[]) {
    pthread_t p[MAX];
    myarg_t args[MAX];
    clock_t start, end;
    start = clock();
    for(int i=0;i<MAX;i++){
        args[i].a = i;
        pthread_create(&(p[i]), NULL, mythread, &args[i]);
    }
    for(int i=0;i<MAX;i++)
    {
        pthread_join(p[i], NULL);
    }
    end = clock();
    printf("Time taken - %f ms\n", (double)((end - start)*1000/CLOCKS_PER_SEC));
    printf("The array populated is \n");
    for(int i=0;i<MAX;i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    return 1;
 }