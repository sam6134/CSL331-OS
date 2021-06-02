# Lab 2

January 27th, 2021<br />

### Covered topics

files, pipes, fork and inter-process communication.

## Background Notes

### Backgroud Note 1

Reading and writing from/to files

```
FILE *fopen(const char *path, const char *mode);
```

### mode:

"r" (read), <br />
"r+" (read+write), <br />
w"w" (truncate file to zero or create text file)<br />
"w+": (read+write)<br />
'a': (append)<br />
'a+':(reading+appending)<br />

```
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
```

```
fread(buff, SIZE, NUMELEM, fd)
```

```
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

```
fwrite(buff,SIZE, strlen(buff),fd)
```

```
int fseek(FILE *stream, long offset, int whence);
```

```
fseek(fd,11,SEEK_CUR)
```

Practice Example for Processing Files:

1. Create an empty file
2. Write numbers 1 to 15 separated by spaces into it
3. Close the file
4. Open the file again
5. Read the first 5 numbers
6. Close the file
7. Open the file again
8. Write numbers 20-25 in the first five locations of the file, overwriting the numbers 1-5
9. Copy all the numbers from 10 and above into an integer array
10. Close the file

### Background Note 2

Learning and Using Fork

1. Use man fork to learn about the fork commands available on your configuration.

2. Practice Example: compile the following code to create a simple child-parent

```
#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
int main(char *argv[], int argc)
{
    pid_t pid;
    int r;
    int a;
    int b;
      // check a and b

    pid=fork();
    // check pid

    if(pid==0)
    { //CHILD PROCESS
            printf("\nChild : %d, %d\n",pid, getpid());
	exit(0);
    }
    else
    { //PARENT PROCESS
            printf("\nParent %d, %d\n",pid, getpid());
           wait();

    }
    return 0;
}
```

### Warmup

Batch processing of commands

**Specification**<br />
A file contains a list of commands, one per each line, with variable number of arguments. Write a C program to read each command and its arguments and execute them. The program terminates when all the commands are completed. <br />

E.g., Sample Commands in the file:

```
ls -al
wc prog.c (replace with your program name)
ping www.google.com
whoami
touch newfile.c
cp prog.c newfile.c
diff prog.c newfile.c
rm prog.c
gcc newfile.c -o newfile.out
```

## Lab Tasks:

### Task 1

**Coordinated non-overlapping writes** <br />

### Task 2

**Use two processes to write into a file.**

### Task 3

**Inter-process communication**

### Task 1

1.  [Parent] Create an empty file
2.  [Parent] Write numbers 1 to 100 separated by spaces into it
3.  [Parent] Close the file
4.  [Child] Create a child process
5.  [Child] Open the file again
6.  [Child] Choose 10 random locations between 1 to 100
7.  [Child] Make those locations Zero
8.  [Child] Close the file and exit
9.  [Parent] Open the file again
10. [Parent] Print the indexes of the locations that have been set to zero by the Child process
11. [Parent] Close file and exit

### Task 2

Repeat the above using exec. To do this, you need to write the logic for the "Child" process separately and compile it into an executable.

1. Run Parent as before.
2. Create a child process and overlay this using Exec. (man exec, execvp, exec, execlp...execve..)
3. Run parent post the overlaid process.

### Task 3

IPC Using Pipes<br />

A pipe is a means of communicating across two different processes.
One process can write to one end of a pipe and the second process can read from the other end. In this example, one process is the child process and the other is the parent process. Repeat the same exercise using pipes.
