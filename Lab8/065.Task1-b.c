#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>

#define MAX 10
#define CONSUMER_MAX 4

pthread_mutex_t index_lock;
pthread_mutex_t index_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

sem_t full[CONSUMER_MAX];


int index_write = 0;
int index_consume = 0;
int arr[MAX];


 typedef struct __myarg_t {
    int a;
 } myarg_t;

 void * produce(void * arg) {
    myarg_t * m = (myarg_t * ) arg;
    int to_write;
    pthread_mutex_lock(&lock);
    to_write = index_write;
    index_write++;
    arr[to_write] = rand();
    printf("Producer thread %d has produced %d at index %d\n", m->a, arr[to_write],to_write);
    sem_post(&full[to_write%CONSUMER_MAX]);
    pthread_mutex_unlock(&lock);
    return NULL;
 }

 void * consume(void * arg)
 {
    myarg_t * m = (myarg_t * ) arg;
    int to_read;
    // choose the index to read from
    pthread_mutex_lock(&index_lock);
    to_read = index_consume;
    index_consume++;
    pthread_mutex_unlock(&index_lock);
    //printf("waiting for semaphore\n");
    for(int i=to_read; i<MAX; i+=CONSUMER_MAX){
    	//printf("%d\n", i);
        sem_wait(&full[to_read]);
        pthread_mutex_lock(&lock);
        int idx = arr[i];
        if(idx == -1 || idx == 0)
        {   
            // refill the semaphore 
            sem_post(&full[to_read]);
            // release the lock for other threads
            pthread_mutex_unlock(&lock);
            continue;
        }
        arr[i] = -1;
        printf("Consuming thread %d consumed %d at index %d\n", m->a,idx,i);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
 }

int main(int argc, char * argv[]) {
    pthread_t p[MAX];
    pthread_t c[MAX];
    myarg_t args[MAX];
    printf("** The MAX variable is set to %d\n", MAX);
    printf("** %d CONSUMERS with each reading from a fixed location ** \n\n", CONSUMER_MAX);
    // initialize MAX condition variables
    for(int i = 0; i < MAX; i++) sem_init(&full[i],0,0);

    
    for(int i=0;i<MAX;i++){
        args[i].a = i;
        pthread_create(&(p[i]), NULL, produce, &args[i]);
    }
    // create consumer threads
    myarg_t args1[CONSUMER_MAX];

    for(int i=0;i<CONSUMER_MAX;i++){
        args1[i].a = i;
        pthread_create(&(c[i]), NULL, consume, &args1[i]);
    }
    // join producers
    for(int i=0;i<MAX;i++) pthread_join(p[i], NULL);

    // posting semaphore just in case of stuck threads (precautionary)
    for(int i=0;i<CONSUMER_MAX;i++) sem_post(&full[i]);
    //printf("joinining consumers\n");
    // join consumers
    for(int i=0;i<CONSUMER_MAX;i++) pthread_join(c[i], NULL);

    return 1;
 }