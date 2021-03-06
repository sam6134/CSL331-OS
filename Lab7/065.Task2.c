#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include<string.h>
 

typedef struct __myarg_t 
{
    int a;
    char *b;
} myarg_t;



typedef struct __myret_t 
{
    int x;
    char *y;

} myret_t;

struct new_struct
{
    int a;
    char* s1;
    char* s2;
};

void * mythread(void * arg) {
    myarg_t * m = (myarg_t * ) arg;
    printf("String for this thread - %s\n\n", m->b);
    myret_t * r = malloc(sizeof(myret_t));
    r->x = 1;
    r->y = m->b;
    return (void * ) r;
}

void * mythread_concat(void * arg) {
    struct new_struct * m = (struct new_struct * ) arg;
    char* new_string = (char*) malloc(strlen(m->s1)+strlen(m->s2)+1);
    printf("In concat thread \n");
    strcat(new_string,m->s1);
    strcat(new_string,m->s2);
    printf("The concat string is - %s\n\n", new_string);
    myret_t * r = malloc(sizeof(myret_t));
    r->x = 1;
    r->y = new_string;
    return (void * ) r;
}
 

int  main(int argc, char * argv[]) 
{
    //int rc;
    pthread_t p1,p2, p3;
    myret_t* m;
    myarg_t args1,args2;
    if(argc < 3)
    {
        printf("\nUsage: ./065.Task2 string1 string2\n\n");
        return -1;
    }
    args1.a= strlen(argv[1]);

    args1.b=(char *)malloc(strlen(argv[1]));

    args2.a = strlen(argv[2]);
    args2.b = (char *)malloc(strlen(argv[2]));

    strcpy(args1.b,  argv[1]);
    strcpy(args2.b,  argv[2]);
    struct new_struct to_pass;
    pthread_create(&p1, NULL, mythread, &args1);
    pthread_create(&p2,NULL,mythread,&args2);
    pthread_join(p1, (void ** ) &m);
    to_pass.s1 = m->y;
    to_pass.a += m->x;
    pthread_join(p2,(void ** ) &m);
    to_pass.s2 = m->y;
    to_pass.a += m->x;
    pthread_create(&p3,NULL,mythread_concat,&to_pass);
    pthread_join(p3,(void ** ) &m);
    printf("returned %d %s\n", m->x, m->y);
    return 0;
}