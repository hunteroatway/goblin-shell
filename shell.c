#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// globals
#define TOKEN_BUFSIZE 64 
#define MAX 128
#define S2(x) #x
#define S(x) S2(x)

// function declarations
char* getCommand();
char** parseCommand(char*);
void printImage();

int main(int argc, char* argv[]) {
  char* cmd;
  char** token;
  char** token2;
  int status; 
  size_t size = 0;
  int serverSet = 0;
  char *pos;
  int port;
  char* username = NULL;
  char* serverName = NULL;
  char* portNo = NULL;

  // parse arguments to gather required information 
  // TODO: should the help code go here??
  if (argc != 7) {
    printf("usage: shell [-u USERNAME] [-s SERVER] [-p PORT] [-h HELP] \n\n");
    printf("required arguments: \n");
    printf("\t-u\tusername information \n");
    printf("\t-s\tserver information \n");
    printf("\t-p\tport information \n\n");
    printf("optional arguments: \n");
    printf("\t-h\thelp information \n\n");
    exit(1);
  } else {
    username = argv[2];
    serverName = argv[4];
    portNo = argv[6];
  }
  
  do {
    printf("goblin-shell > ");
    cmd = getCommand();
    token = parseCommand(cmd);
  
    // check to see if user wants help or exit
    if(!strcmp(token[0], "exit") || !strcmp(token[0], "lo") || !strcmp(token[0], "quit") || !strcmp(token[0], "shutdown")){
      printf("goblin-shell terminating...\n");
      printImage();
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(token[0], "help") || !strcmp(token[0], "h")){
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
    }  else if (!strcmp(token[0], "compile")){
      // set up compiling
      //first check to see if server is set up
      if(serverSet == 1) {
        // invoke client with form ./client $serverName $Port compile $[compliation]
        token2 = malloc(sizeof(char*)*5);
        token2[0] = strdup("client");
        token2[1] = strdup(username);
        token2[2] = strdup(serverName);
        token2[3] = strdup(portNo);
        token2[4] = strdup("compile");
        //merge arrays
        char** command = malloc(sizeof(char*)*(TOKEN_BUFSIZE+5));
        memcpy(command, token2, sizeof(char*)*5);
        memcpy(command+5, token, sizeof(char*)*TOKEN_BUFSIZE);

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
        // print they need to set server
        printf("You need to set a server to connect to. Use \"setServer\" to declare the target server.\n");
      }

    } else if (!strcmp(token[0], "run")){
      // set up compiling
      //first check to see if server is set up
      if(serverSet == 1) {
        // invoke client with form ./client $username $serverName $Port compile $[compliation]
        token2 = malloc(sizeof(char*)*5);
        token2[0] = strdup("client");
        token2[1] = strdup(username);
        token2[2] = strdup(serverName);
        token2[3] = strdup(portNo);
        token2[4] = strdup("run");
        //merge arrays
        char** command = malloc(sizeof(char*)*(TOKEN_BUFSIZE+5));
        memcpy(command, token2, sizeof(char*)*5);
        memcpy(command+5, token, sizeof(char*)*TOKEN_BUFSIZE);

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
        // print they need to set server
        printf("You need to set a server to connect to. Use \"setServer\" to declare the target server.\n");
      }

    }else if (!strcmp(token[0], "setServer") || !strcmp(token[0], "set") && !strcmp(token[1], "Server")){
      // set up the server
      printf("Enter the host name: ");
      fflush(stdout);
      getline(&serverName, &size, stdin);
      fflush(stdout);
      printf("Enter the port number: ");
      getline(&portNo, &size, stdin);
      // change newline to end of line
      if ((pos=strchr(serverName, '\n')) != NULL)
        *pos = '\0';
      if ((pos=strchr(portNo, '\n')) != NULL)
        *pos = '\0';

      //cast port to int
      port = atoi(portNo);

      //check for valid port
      if(port < 65535 && port > 1024)
        serverSet = 1;
      else 
      printf("Invalid port. Please try again with a new port number. \n");
    }else {
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
