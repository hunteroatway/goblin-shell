#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOKEN_BUFSIZE 64 
#define MAX 128

char* get_command();
char** parse_command(char*);
void printImage();

int main(int argc, char* argv[]) {
  char* cmd, pos;
  char* username = NULL;
  char* serverName = NULL;
  char* portNo = NULL;
  char** token;
  int status, port; 
  size_t size = 0;

   // parse arguments to gather required information 
  if (argc != 7) {
    printf("usage: shell [-u USERNAME] [-s SERVER] [-p PORT] \n\n");
    printf("required arguments: \n");
    printf("\t-u\tusername information \n");
    printf("\t-s\tserver information \n");
    printf("\t-p\tport information \n\n");
    exit(1);
  } else {
    username = argv[2];
    serverName = argv[4];
    portNo = argv[6];
    port = atoi(portNo);
  }
  
  do {
    printf("goblin-shell > ");
    cmd = get_command();
    token = parse_command(cmd);

    // check to see if user wants help or exit
    if (!strcmp(token[0], "exit") || !strcmp(token[0], "lo") || !strcmp(token[0], "quit") || !strcmp(token[0], "shutdown")) {
      printf("goblin-shell terminating...\n");
      printImage();
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(token[0], "help") || !strcmp(token[0], "h")) {
        printf("usage: goblin-shell [COMMANDS] \n\n");
        printf("commands: \n");
        printf("\tcopy\t  copy a file to the remote host \n");
        printf("\tcompile\t  compile a file on the remote host \n");
        printf("\trun\t  run a program on the remote host \n");
        printf("\thelp\t  display usage information \n");
        printf("\texit\t  exit the shell \n\n");
    } else if (!strcmp(token[0], "compile")) {
      // invoke client with form ./client username servername port compile
      char** command = malloc(sizeof(char*)*5);
      command[0] = strdup("client");
      command[1] = strdup(username);
      command[2] = strdup(serverName);
      command[3] = strdup(portNo);
      command[4] = strdup("compile");

      // create a child to exec the command
      pid_t child = fork();
      if(child > 0) { // parent
        wait(&status);
        free(command);
      } else if (child == 0) { //child
        printf("testing here...");
        execv("./client", command);
        perror("Failed to exec. Type help for more information on usage");
        exit(0);
      } else { // failed to fork
        perror("Failed to fork");
      }
    } else if (!strcmp(token[0], "run")){
        // invoke client with form ./client username servername port run
        char** command = malloc(sizeof(char*)*5);
        command[0] = strdup("client");
        command[1] = strdup(username);
        command[2] = strdup(serverName);
        command[3] = strdup(portNo);
        command[4] = strdup("run");
        
        // create a child to exec the command
        pid_t child = fork();
        if(child > 0) { // parent
          wait(&status);
          free(command);
        } else if (child == 0) { //child
          execv("./client", command);
          perror("Failed to exec. Type help for more information on usage");
          exit(0);
        } else { // failed to fork
          perror("Failed to fork");
        }
    } else printf("Invalid command. Please try again. Type \"help\" for more information.");
    

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
  }

  char read[MAX];
  while(fgets(read, sizeof(read), file) != NULL)
    printf("%s", read);

    fclose(file);
}