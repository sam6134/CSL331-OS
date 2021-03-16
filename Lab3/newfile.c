#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

int main()
{
  FILE *fp;
  fp = fopen("test.txt", "w+");
  for(int i=1;i<=15;i+=1)
  {
      putw(i, fp);
      fputc(' ',fp);
  }
  fclose(fp);
  fp = fopen("test.txt", "r");
  fseek(fp,0,SEEK_SET);
  for(int i=0;i<5;i++){
      int x = getw(fp);
      printf("%d ",x);
      char dummy = getc(fp);  
  }
  fclose(fp);
  fp = fopen("test.txt", "r+");
  fseek(fp,0,SEEK_SET);
  for(int i=20;i<=25;i++)
  {
      putw(i, fp);
      fputc(' ', fp);
  }
  fclose(fp);
  fp = fopen("test.txt", "r");
  fseek(fp,0,SEEK_SET);
  int* arr = (int*)malloc(12*sizeof(int));
  int j = 0;
  for(int i=0;i<15;i++)
  {
      int x = getw(fp);
      //printf("%d ", x);
      if(x>10)
      {
          arr[j] = x;
          j++;
      }
      char dummy = getc(fp);
  }
  fclose(fp);
  for(int i=0;i<11;i++)
  {
      printf("%d ", arr[i]);
  }
}