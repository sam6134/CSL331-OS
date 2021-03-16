
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Read characters from the pipe and echo them to stdout. */
void read_from_pipe (int file)
{
/* the variable file is of integer type and
it points a memory location that can be written 
into*/

  FILE *stream;
  int c,x;
  stream = fdopen (file, "r");
  while(fread(&x, 4, 1, stream)){
    printf("Recieved flag %d from pipe\n", x);}
  fclose (stream);
}

/* Write some a flag=1 to specify that were are 
done with writing locations*/

void write_to_pipe (int file)
{
/* the variable file is of integer type and
it points a memory location that can be written 
into*/

  FILE *stream;
  stream = fdopen (file, "w");
  int i=1;
  fwrite(&i, 4, 1, stream);
  fclose (stream);
}

int main (void)
{
  printf("Task-3 Getting positions set by child to zero from parent using pipe for inter-process communication\n");
    printf("The child sends a flag 1 through pipe, once parent recieves it starts reading the file (Thus we don't need to use the wait command)\n\n");
  FILE *fp;
  fp = fopen("test.txt", "w+");
  srand(time(0));
    // write 1 to 100
  for(int i=1;i<=100;i++)
  {
    putw(i, fp);
    fputc(' ',fp);
  }
  fclose(fp);
  pid_t pid;
  int mypipe[2];

  /* Create the pipe. */
  if (pipe (mypipe))
    {
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
    }
  /* Create the child process. */
  pid = fork ();
  if (pid == (pid_t) 0)
    {
      /* This is the child process.
         Close other end first. */
      fp = fopen("test.txt", "r+");
      fseek(fp,0,SEEK_SET);
      printf("Random integers set by child are-\n");
            
      for(int i=0;i<10;i++){
        int randIdx = rand()%100;
        printf("%d ", randIdx);
        fseek(fp,(sizeof(int)+sizeof(char))*randIdx,SEEK_SET);
        putw(0, fp);
      }
      fclose(fp);
      printf("\n");
      //printf("Child : %d, %d\n",pid, getpid());
      close(mypipe[0]);
      write_to_pipe(mypipe[1]);
      return EXIT_SUCCESS;
    }
  else if (pid < (pid_t) 0)
    {
      /* The fork failed. */
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
    }
  else
    {
      /* This is the parent process.
         Close other end first. */
      close(mypipe[1]);
      read_from_pipe(mypipe[0]);
      fp = fopen("test.txt", "r");
      printf("Random integers from parents are-\n");
      for(int i=0;i<100;i++){
          int x = getw(fp);
          char dummy = getc(fp);
          if(x == 0)
          {
            printf("%d ",i);
          }
      }
      printf("\n");
      //printf("\nParent %d, %d\n",pid, getpid());
      fclose(fp);
      return EXIT_SUCCESS;
    }
}



