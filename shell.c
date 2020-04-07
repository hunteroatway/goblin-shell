#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

// global definitions
#define TOKEN_BUFSIZE 64
#define MAX 128
#define S2(x) #x
#define S(x) S2(x)

// globals
unsigned short SERV_RUNNING = 0;

// function declarations
char* getCommand();
char** parseCommand(char*);
void printImage();
void startServer(int, char*, char*);

int main(int argc, char* argv[]) {
  char* cmd;
  char** token;
  int status; 
  size_t size = 0;
  char *pos;
  int port;
  char* username = NULL;
  char* serverName = NULL;
  char* portNo = NULL;

  // parse arguments to gather required information 
  if (argc != 7) {
    printf("usage: goblin-shell [-u USERNAME] [-s SERVER] [-p PORT] \n\n");
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
    //if (SERV_RUNNING == 0)
      //startServer(status, username, serverName);
    
    printf("goblin-shell > ");
    cmd = getCommand();
    token = parseCommand(cmd);
  
    // check to see if user wants help or exit
    if (!strcmp(token[0], "exit") || !strcmp(token[0], "lo") || !strcmp(token[0], "quit") || !strcmp(token[0], "shutdown")) {
      printf("Goodbye! \n");
      //printImage();
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(token[0], "help") || !strcmp(token[0], "h")) {
      // print some helpful stuff
      printf("usage: goblin-shell [COMMANDS] \n\n");
      printf("commands: \n");
      printf("\tcopy\t  copy a file to the remote host \n");
      printf("\tcompile\t  compile a file on the remote host \n");
      printf("\trun\t  run a program on the remote host \n");
      printf("\thelp\t  display usage information \n");
      printf("\texit\t  exit the shell \n\n");
    } else if (!strcmp(token[0], "compile")) {
        // client $username $serverName $Port compile $[compliation]
        char** command = malloc(sizeof(char*)*(TOKEN_BUFSIZE+5));
        command[0] = strdup("client");
        command[1] = strdup(username);
        command[2] = strdup(serverName);
        command[3] = strdup(portNo);
        memcpy(command+4, token, sizeof(char*)*TOKEN_BUFSIZE);

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
    } else if (!strcmp(token[0], "run")) {
        //./client $username $serverName $Port run $[program & arguments]
        char** command = malloc(sizeof(char*)*(TOKEN_BUFSIZE+5));
        command[0] = strdup("client");
        command[1] = strdup(username);
        command[2] = strdup(serverName);
        command[3] = strdup(portNo);
        memcpy(command+4, token, sizeof(char*)*TOKEN_BUFSIZE);

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
    } else {
      printf("Invalid command. Please try again. Type \"help\" for more information. \n");
    }

    free(cmd);
    free(token);
  } while (1);

  return EXIT_SUCCESS;
}

char* getCommand() {
  char* cmd = NULL;
  size_t size = 0;
  getline(&cmd, &size, stdin);
  return cmd;
}

char** parseCommand(char* _cmd) {
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
  char read[MAX];

  if((file = fopen("goblin.txt", "r")) == NULL){
    perror("Error opening file");
    return;
  }

  while(fgets(read, sizeof(read), file) != NULL)
    printf("%s", read);
    return;
    fclose(file);
}

void startServer(int status, char* uname, char* server) {
  pid_t child = fork();

  char copyPath[256] = {0};
  strcat(copyPath, uname);
  strcat(copyPath, "@");
  strcat(copyPath, server);
  strcat(copyPath, ":");
  strcat(copyPath, "~/hello.c");

  char sshPath[256] = {0};
  strcat(sshPath, uname);
  strcat(sshPath, "@");
  strcat(sshPath, server);

  if(child > 0) 
    wait(&status);
  else if (child == 0) {
    // TODO: These will not run if both are un-commented. Need to fix this. Maybe multiple processes (double fork)?
    //execl("/usr/bin/scp", "scp -q", "hello.c", copyPath, NULL);
    //execl("/usr/bin/ssh", "ssh", sshPath, "\'gcc\'", "\'hello.c\'", "\'-o\'", "\'hello\'", NULL);
    //execl("/usr/bin/ssh", "ssh", sshPath, "\'hello\'", "\'>\'", "\'.\\exec-log.txt\'", NULL);
    execl("/usr/bin/ssh", "ssh", sshPath, "\'hello\'", NULL);

    perror("Failed to exec. Type help for more information.");
    exit(0);
  } 
  else perror("failed to fork");

  SERV_RUNNING = 1;
}