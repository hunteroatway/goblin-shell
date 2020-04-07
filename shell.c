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
    if (SERV_RUNNING == 0)
      startServer(status, username, serverName);
    
    printf("goblin-shell > ");
    cmd = getCommand();
    token = parseCommand(cmd);
  
    // check to see if user wants help or exit
    if (!strcmp(token[0], "exit") || !strcmp(token[0], "lo") || !strcmp(token[0], "quit") || !strcmp(token[0], "shutdown")) {
      printf("goblin-shell terminating...\n");
      printImage();
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(token[0], "help") || !strcmp(token[0], "h")) {
      // print some helpful stuff
      printf("This shell is designed to allow the user to designate various code files to be remotely compiled and executed for testing. \n");
      printf("You can set the server to compile on by using the command \"setServer\" and filling out the host name and port address.\n");
      printf("The server program will have to be running on the target server in order for the remote compile to work.\n");
      printf("Use the command \"compile\" followed by the files to have them remotely compiled on the chosen server. \n");
      printf("Usage: compile (files) (flags)");
      printf("Use the command \"run\" to run the compiled program on the remote system. (You will have to send the files to compile prior to using run) \n");
      printf("Usage: run (programName) (arguments)");
      printf("You can also use this shell in order to execute any commands from a unix system locally. Example usage is ./example.c arg1 arg2 \n");
      printf("To quit the shell type \"exit, lo, quit or shutdown\". \n");
    }  else if (!strcmp(token[0], "compile")) {
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
          execv("./client", command);
          perror("Failed to exec. Type help for more information on usage");
          exit(0);
        } else { // failed to fork
          perror("Failed to fork");
        }
    } else if (!strcmp(token[0], "run")) {
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
    execl("/usr/bin/ssh", "ssh", sshPath, "\'hello\'", NULL);

    perror("Failed to exec. Type help for more information.");
    exit(0);
  } 
  else perror("failed to fork");

  SERV_RUNNING = 1;
}