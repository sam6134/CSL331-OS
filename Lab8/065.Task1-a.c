#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 10
#define CONSUMER_MAX 4

pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

sem_t full;


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
    index_write = (index_write + 1)%MAX;
    arr[to_write] = rand();
    printf("Producer thread %d has produced %d at index %d\n", m->a, arr[to_write],to_write);
    pthread_mutex_unlock(&lock);
    sem_post(&full);
    return NULL;
 }

 void * consume(void * arg)
 {
    myarg_t * m = (myarg_t * ) arg;
    for(int i=0; i<MAX; i++){
        sem_wait(&full);
        pthread_mutex_lock(&lock);
        int idx = arr[i];
        // location already consumed or not yet filled
        if(idx == -1  || idx == 0){
            pthread_mutex_unlock(&lock);
            // refill the semaphore since we have not consumed
            sem_post(&full); 
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
    printf("** %d CONSUMERS with each reading from ANY location ** \n\n", CONSUMER_MAX);
    // initialize the semaphores 
    sem_init(&full,0,0);

    
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

    //send the semaphore to stuck threads (if any) (precautionary)
    for(int i=0;i<CONSUMER_MAX;i++) sem_post(&full);
    //printf("joining consumers\n");
    // join consumers
    for(int i=0;i<CONSUMER_MAX;i++) pthread_join(c[i], NULL);

    return 1;
 }