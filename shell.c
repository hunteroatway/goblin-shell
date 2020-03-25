#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOKEN_BUFSIZE 64 

char* get_command();
char** parse_command(char*);

int main(int argc, char* argv[]) {
  char* cmd;
  char** token;
  int status; 

  do {
    printf("goblin-shell > ");
    cmd = get_command();
    token = parse_command(cmd);

    // create a child to exec the command
    pid_t child = fork();
    if(child > 0) { // parent
      wait(&status);
    } else if (child == 0) { //child
      execvp(cmd, token);
      perror("Failed to exec");
      exit(0);
    } else { // failed to fork
      perror("Failed to fork");
    }

    free(cmd);
    free(token);
  } while (1);

  return EXIT_SUCCESS;
}

char* get_command() {
  char* cmd = NULL;
  size_t size = 0;
  getline(&cmd, &size, stdin);
  return cmd;
}

char** parse_command(char* _cmd) {
  int size = TOKEN_BUFSIZE;
  int pos = 0;  
  char* delim = " \n\t";
  char** buffer = malloc(sizeof(char*)*size);
  char* token;

  if (!buffer) {
    printf("Bad allocation error!");
    exit(EXIT_FAILURE);  
  }

  token = strtok(_cmd, delim);

  while (token != NULL) {
    buffer[pos] = token;
    pos++;

    if (pos >= size) {
      size += TOKEN_BUFSIZE;
      buffer = realloc(buffer, sizeof(char*)*size); 

      if (!buffer) {
        printf("Bad allocation error!");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, delim);  
  }

  buffer[pos] = NULL;
  return buffer;
}
