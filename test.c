#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h> 

int main(int argc, char* argv[]) {
  printf("Here are the args passed: %s, %s \n", argv[1], argv[2]);

  exit(0);

}