# Lab 7

**Programming with Threads : Creating and Waiting**

## Task 0

**To create a basic multi-threaded program that will print its arguments. Use Chapter 27 from Textbook for further reference.** <br/>
Learn to use (refer man pages):

**pthread_create function** (see the following code for usage)

```
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

To do: <br />
a) Compile the above code and run it. To compile, you need to link with the pthread library:

```
gcc thread.c -o thread -Wall -pthread
```

What is the output you get?

b) If "hello world" doesn't print, make it print without using pthread_join?

c) Make "hello world" appear with pthread_join (see next task for more complex usage of pthread_join).

## Task 2

Waiting for you! Using pthread_join. This piece of code is a modified version of the code from Chapter 27. You will learn to use pthread_join and to manipulate the arguments returned by pthread_join

```
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



void * mythread(void * arg) {

myarg_t * m = (myarg_t * ) arg;


int print_index=0;



for(print_index=0; print_index<m->a; print_index++)
{


printf("Printing %d th character %c\n", print_index, *(m->b+print_index));


}


myret_t * r = malloc(sizeof(myret_t));


r->x = 1;

r->y = m->b;

return (void * ) r;

}



int  main(int argc, char * argv[])
{

int rc;

pthread_t p;
myret_t * m;


myarg_t args;

args.a= strlen(argv[1]);

args.b=(char *)malloc(strlen(argv[1]));

strcpy(args.b,  argv[1]);


pthread_create(&p, NULL, mythread, &args);


pthread_join(p, (void ** ) &m);


printf("returned %d %s\n", m->x, m->y);


return 0;


}
```

To do: <br/>
a) Compile the above code. There may be minor errors (possible), correct them yourself. <br />
b) Create two more threads. The main program will take one more additional string as input. <br/>
c) The second thread will print the second string.<br />
d) The third thread will concatenate the first and second strings (with an additional space character in between) and print the final result. For example, first thread could process "hello", second thread could process "world" and third thread will process "hello world".

### Constraints

The concatenation should not be done in the main program. Create another structure whose members will be initialized by the return values of the two threads. This new structure will be passed to the third thread for the desired output.

## Programming with Threads

Locking

Template locking code <br />

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

Compiling: <br/>
Need to link with the pthreads library.

```
gcc -o main main.c -Wall -pthread
```

## Task 3

Filling an array with random integers using multiple threads using single mutex.

Use above code as template <br/>
a. Initialize a null array of integers of size MAX <br/>
b. Create MAX threads <br />
c. Each thread should generate a random value (need not be distinct) and fill a distinct non-NULL location in the array.<br />
d. No two threads should fill the same location, use locks to synchronize the threads appropriately

## Task 4

Achieve the same using one or more conditional variables.

Question: Which code runs faster?

## (Advanced) Task 3

Create some consumer threads that will read from the array. Two versions are possible.

a. A consumer thread can read from any array location
b. A consumer thread only reads from a specific array location
