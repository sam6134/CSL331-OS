#include<stdio.h>
#include <pthread.h>
#include <unistd.h>

 typedef struct __myarg_t {
    int a;
    char *b;
 } myarg_t;
int running_thread;
 void * mythread(void * arg) {
    myarg_t * m = (myarg_t * ) arg;
    printf("%d %s\n", m->a, m->b);
    running_thread--;
    return NULL;
 }

int main(int argc, char * argv[]) {
    pthread_t p;
    running_thread = 0;
    myarg_t args;
    args.a = 10;
    args.b = "hello world";
    running_thread++;
    pthread_create(&p, NULL, mythread, &args);
    
    while (running_thread>0)
        ;
    // for using join
    // pthread_join(p,NULL);
    return 1;
 }