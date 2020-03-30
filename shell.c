
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOKEN_BUFSIZE 64 
#define MAX 128

// used for getting the value of MAX inside the scanf. Converts max to the literal x to be concatinated
#define S2(x) #x
#define S(x) S2(x)

char* get_command();
char** parse_command(char*);
void printImage();

int main(int argc, char* argv[]) {
  char* cmd;
  char** token;
  int status; 
  size_t size = 0;
  char* username = NULL;

  //prompt for username
  printf("Welcome to goblin-shell. Please enter your username to continue: \n");
  getline(&username, &size, stdin);
  printf("goblin-shell Welcome %s\n", username);
  username[strlen(username)-1] = 0; 
 
  do {
    printf("goblin-shell > ");
    cmd = get_command();
    token = parse_command(cmd);
    char* first = malloc(sizeof(char*));
    strcpy(first, token[0]);

    // check to see if user wants help or exit
    if(!strcmp(first, "exit") || !strcmp(first, "lo") || !strcmp(first, "quit") || !strcmp(first, "shutdown")){
      printf("goblin-shell terminating...\n");
      printImage();
      free(first);
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(first, "help") || !strcmp(first, "h")){
      // print some helpful stuff
      printf("You can use this shell in order to execute any commands from a unix system. Example usage is ./example.c arg1 arg2 \n");
      printf("To quit the shell type \"exit, lo, quit or shutdown\". \n");
    } else {
      // create a child to exec the command
      pid_t child = fork();
      if(child > 0) { // parent
        wait(&status);
      } else if (child == 0) { //child
       
        // setup file path 
        char path[256];
        strcat(path, username);
        strcat(path, "@");
        strcat(path, "hercules.cs.uregina.ca");
        strcat(path, ":");
        strcat(path, "~/temp-shell");
  
        // copy the server file over to the remote host
        execl("/usr/bin/scp", "scp -q", "server.c", path, NULL);

        perror("Failed to exec. Type help for more information on usage");
        exit(0);
      } else { // failed to fork
        perror("Failed to fork");
      }
    }

    free(first);
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

void printImage() {

  FILE *file = NULL;
  if((file = fopen("goblin.txt", "r")) == NULL){
    perror("Error opening file");
    return;
  }

  char read[MAX];
  while(fgets(read, sizeof(read), file) != NULL)
    printf("%s", read);

    return;

    fclose(file);
}
