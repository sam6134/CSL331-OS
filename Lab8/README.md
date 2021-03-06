# Lab 8

**Programming with Threads : Locking**

Template locking code -

```
//lock declaration functions
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

//declaration
pthread_mutex_t lock;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; (or dynamic if you prefer)

pthread_mutex_lock(&lock);
x = x + 1; // or whatever your critical section is
pthread_mutex_unlock(&lock);


#include<stdio.h>
#include <pthread.h>

 typedef struct __myarg_t {
 int a;
 char *b;
 } myarg_t;

 void * mythread(void * arg) {
 myarg_t * m = (myarg_t * ) arg;
 printf("%d %s\n", m->a, m->b);
 return NULL;
 }

int main(int argc, char * argv[]) {
pthread_t p;

myarg_t args;
args.a = 10;
args.b = "hello world";

pthread_create(&p, NULL, mythread, &args);
return 1;
 }
```

Compiling:<br/>
Need to link with the pthreads library.

```
gcc -o main main.c -Wall -pthread
```

## Prologue Task 1

Filling an array with random integers using multiple threads using single mutex.

Use above code as template <br />
a. Initialize a null array of integers of size MAX <br />
b. Create MAX threads <br />
c. Each thread should generate a random value (need not be distinct) and fill a distinct non-NULL location in the array. <br />
d. No two threads should fill the same location, use locks to synchronize the threads appropriately

## Present Task 1

Create some consumer threads that will read from the array. Two versions are possible.

a. A consumer thread can read from any array location. <br />
b. A consumer thread only reads from a specific array location

## Task 2: Coordinated logging

A network application has multiple threads to monitor activity on multiple network ports. When a thread completes the task it "logs" the event to dashboard (screen) or a file. However, each thread writes a different length string to the screen. Write a program that ensures that no two threads end up writing overlapping strings to the screen.
The following constraints apply:

1. Each thread picks a global "serial number" prior to beginning the monitoring. (The global serial number
   is used a mechanism by the main process to track that thread's progress. The TID and the global serial number are
   maintained in a log book by the main thread.)

2. The threads should write in an ascending order, that is, a thread with lower serial number should write first
   and then the thread with higher serial number (next in sequence) follows.

You can use any type of mechanis, mutexes or conditional variables or even semaphores to do this task.
