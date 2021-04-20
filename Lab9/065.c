#include<stdio.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>

#define MAX 10000
#define M 10

// define locks
pthread_mutex_t array_lock;
pthread_mutex_t array_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t stop_lock;
pthread_mutex_t stop_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t stop_search_lock;
pthread_mutex_t stop_search_lock = PTHREAD_MUTEX_INITIALIZER;

// declare semaphores
sem_t search_signal[MAX];
sem_t full[MAX];
sem_t empty[MAX];

// global variables for stopping sorting and searching 
int stopsort = 0;
int stopsearch = 0;

// search array struct
struct search_array{
    int* arr;
    int size;
};

// pushed array stores all the array available
struct search_array* pushed_arrays;
// global indexes for keeping track of indexes
int* query_integers;
int global_index = 0;
int sort_index = 0;

// args for file reading threads
 typedef struct __myarg_t {
    FILE *fp;
 } myarg_t;

// argument for searching threads
struct search_arg{
    int id;
};

// comparator for quicksort
int comparator(const void *p, const void *q) 
{ 
      return (*(int*)p-*(int*)q);
} 


// bring array function that fetches array after random delays
 void * bring_array(void * arg) {
    myarg_t * m = (myarg_t * ) arg;
    FILE* fp = m->fp;
    char * line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
        // sleep for random time
        usleep((1 + rand()%100)*100);
        if(line[0] == ' ') continue;
        // printf("%s", line);
        char* orig = (char *)malloc(sizeof(char)*(strlen(line)+1));
        strcpy(orig, line);
        char* token;
        int len = 0;
        // extract integers since they are space separated
        token = strtok(line, " ");
        while( token != NULL ) {
        token = strtok(NULL, " ");
            len++;
        }
        // allocate space for new array
        pushed_arrays[global_index].arr = (int *) malloc(sizeof(int)*len);
        token = strtok(orig," ");
        // assign values one by one
        for(int i=0;i<len;i++)
        {
            pushed_arrays[global_index].arr[i] = atoi(token);
            token = strtok(NULL, " ");
        }
        pushed_arrays[global_index].size = len;
         
        global_index++;
    }
    // after all arrays are fetched set stopsort = 1
    pthread_mutex_lock(&stop_lock);
    stopsort = 1;
    pthread_mutex_unlock(&stop_lock);
    return NULL;
 }


// the sort flow for sorting the fetched array one by one
void* sort_flow(void * arg)
{
    while(1){
        // check stopping condition
        pthread_mutex_lock(&stop_lock);
        if(stopsort == 1 && sort_index >= global_index) 
        {
            //printf("stopping sorting\n");
            break;
        }
        pthread_mutex_unlock(&stop_lock);
        // spin till global index not greater sort index
        while(sort_index >= global_index) ;
        qsort((void*)pushed_arrays[sort_index].arr, pushed_arrays[sort_index].size, sizeof(pushed_arrays[sort_index].arr[0]), comparator);
        //printf("sorted array %d\n", sort_index);
        sem_post(&search_signal[sort_index]); 
        pthread_mutex_lock(&array_lock);
        sort_index ++;
        pthread_mutex_unlock(&array_lock);
    }
    return NULL;
}

// search flow for searching a value one by one
void* search_flow(void * arg)
{
    struct search_arg* m = (struct search_arg*) arg;
    //printf(" wait for search signal ( or till array sorts )\n");
    
    sem_wait(&search_signal[m->id]);

    while(1)
    {
        //printf("wait for query data at %d\n", m->id);
        sem_wait(&full[m->id]);

        // stopping condition
        pthread_mutex_lock(&stop_search_lock);
        if(stopsearch == 1 && query_integers[m->id] == -1) break;
        if(query_integers[m->id] == -1) break;
        pthread_mutex_unlock(&stop_search_lock);

        int key = query_integers[m->id];
        // binary search for the value
        int* res = bsearch(&key,pushed_arrays[m->id].arr,pushed_arrays[m->id].size,sizeof(pushed_arrays[m->id].arr[0]),comparator);
        if(res == NULL)
        {
            // value not found
            printf("Search thread with ID %d could not find %d in arr[%d]\n",m->id,key,m->id);
        }else{
            // value found
            printf("Search thread with ID %d found %d  at location %d in arr[%d]\n",m->id,key,(int)(res - pushed_arrays[m->id].arr),m->id);
        }
        // query processing done
        // post the empty signal and consume the query integer
        query_integers[m->id] = -1;
        sem_post(&empty[m->id]);
    }
    return NULL;
}


// read query flow for reading the queries one by one
void * read_query(void * arg){
    myarg_t * m = (myarg_t * ) arg;
    FILE* fp = m->fp;
    char * line = NULL;
    size_t len = 0;
    // start fetching the query_integers
    while (getline(&line, &len, fp) != -1) {
        // add random sleeps for random scheduling
        usleep((1 + rand()%100)*100);
        // read integer
        int q = atoi(line);
        // check the number of available arrays
        pthread_mutex_lock(&array_lock);
        int n = sort_index;
        pthread_mutex_unlock(&array_lock);
        // for each thread send a signal
        for(int i=0;i<n;i++)
        {
            // wait for empty signal
            sem_wait(&empty[i]);
            // add the query_integer
            query_integers[i] = q;
            sem_post(&full[i]);
            // post the full signal
        }
    }
    // set the stop search signal after all queries
    pthread_mutex_lock(&stop_search_lock);
    stopsearch = 1;
    pthread_mutex_unlock(&stop_search_lock);
    return NULL;
}

int main(int argc, char * argv[]) {
    printf("\n");
    printf("**************************************************************\n\n");
    printf("CSL331 Lab-9 (Workflow) © Samarth Singh\n\n");
    printf("**************************************************************\n\n");

    pthread_t p;
    // pthread_t c[MAX];
    myarg_t args;
    FILE* arrayfile;
    FILE* queryfile;
    pushed_arrays =  (struct search_array *) malloc(sizeof(struct search_array)* MAX);
    query_integers = (int *) malloc(sizeof(int)*MAX);
    arrayfile = fopen("arraydata.txt", "r");
    queryfile = fopen("querydata.txt", "r");

    if (arrayfile == NULL || queryfile == NULL)
        exit(EXIT_FAILURE);
    // initialize MAX semaphores variables
    for(int i = 0; i < MAX; i++) sem_init(&search_signal[i],0,0);
    for(int i = 0; i < MAX; i++) sem_init(&empty[i],0,1);
    for(int i = 0; i < MAX; i++) sem_init(&full[i],0,0);

    // create thread for dynamically reading an array
    args.fp = arrayfile;
    pthread_create(&(p), NULL, bring_array, &args);


    // create a sort thread
    pthread_t psearch;
    pthread_create(&(psearch), NULL, sort_flow, &args);

    // create MAX search threads
    pthread_t p1[MAX];
    struct search_arg arg1[MAX];
    for(int i = 0; i < MAX; i++)
    {
       arg1[i].id = i;
       pthread_create(&(p1[i]), NULL, search_flow, &(arg1[i]));
    }
    
    pthread_t pquery;
    myarg_t args2;
    args2.fp = queryfile;
    pthread_create(&(pquery), NULL, read_query, &args2);

    // join array collection thread
    pthread_join(p, NULL);

    // join array sorting thread
    pthread_join(psearch, NULL);

    // join query reading thread
    pthread_join(pquery, NULL);
    // precautionary measure to get rid of stuck threads

    //printf("joing search threads\n");
    for(int i = 0; i < MAX; i++) 
    {
    	pthread_cancel(p1[i]);
    }

    return 1;
 }