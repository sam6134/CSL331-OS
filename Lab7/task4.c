#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <stdlib.h>

#define MAX 10000
pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condvar = PTHREAD_COND_INITIALIZER;
int index_write = 0;
int arr[MAX];
int is_writing = 0;
 typedef struct __myarg_t {
    int a;
 } myarg_t;

 void * mythread(void * arg) {
    // myarg_t * m = (myarg_t * ) arg;
    int to_write;
    // printf("Thread is waiting %d\n", m->a);
    pthread_mutex_lock(&lock);
    while(is_writing){
        pthread_cond_wait (&condvar, &lock);
    }
    // printf("Thread has waited %d\n", m->a);
    is_writing = 1;
    pthread_mutex_unlock(&lock);
    to_write = index_write;
    index_write++;
    is_writing = 0;
    pthread_cond_signal (&condvar);
    
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