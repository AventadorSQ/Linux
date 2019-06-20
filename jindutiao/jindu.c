#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main()
{
  int i = 0;
  char buff[102];
  memset(buff, 0, sizeof(buff));
  const char* ptr = "|/-\\";
  while(i <= 100)
  {
    printf("[%-100s][%d%%][%c]\r", buff, i, ptr[i%4]);
    fflush(stdout);
    buff[i++] = '#';
    usleep(50000);
  }
  printf("\n");
  return 0;
}
