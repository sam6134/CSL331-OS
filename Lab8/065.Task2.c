#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>

#define MAX 30
pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t serial_lock;
pthread_mutex_t serial_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t  condvar = PTHREAD_COND_INITIALIZER;
int global_turn = 0;
int global_serial_id = 0;

 typedef struct __myarg_t {
    int tid;
 } myarg_t;

 void * mythread(void * arg) {
    int serial_id;
    pthread_mutex_lock(&serial_lock);
    serial_id = global_serial_id;
    global_serial_id++;
    pthread_mutex_unlock(&serial_lock);

    myarg_t * m = (myarg_t * ) arg;
    pthread_mutex_lock(&lock);
    while(serial_id != global_turn)
    {
        printf("Not my turn %d\n", serial_id);
        pthread_cond_wait (&condvar, &lock);
    }
    printf("I am thread  with thread no. %d with turn %d and logging my statement\n", m->tid ,serial_id);
    global_turn++;
    pthread_cond_broadcast (&condvar);
    pthread_mutex_unlock(&lock);
    // printf("Thread is writing %d\n", m->a);
    return NULL;
 }

int main(int argc, char * argv[]) {

    printf("**COORDINATED LOGGING**\n");
    printf("The MAX thread spawned are - %d\n\n\n", MAX);

    
    pthread_t p[MAX];
    myarg_t args[MAX];
    // clock_t start, end;
    for(int i=0;i<MAX;i++){
        args[i].tid = i;
        pthread_create(&(p[i]), NULL, mythread, &args[i]);
    }
    for(int i=0;i<MAX;i++)
    {
        pthread_join(p[i], NULL);
    }
    return 1;
 }