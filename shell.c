#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// function declarations
char* getCommand();
char** parseCommand(char*);
void printImage();
int startServer(char* username, char* server, int, int);

// global constants
const unsigned int TOKEN_BUFSIZE = 64; 
const unsigned int MAX = 128;
const unsigned int OFFSET = 4;

int main(int argc, char* argv[]) {
  // variable definitions
  char* cmd, pos;
  char* username = NULL;
  char* servername = NULL;
  char* portno = NULL;
  char** token;
  int status, port; 
  size_t size = 0;

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
    servername = argv[4];
    portno = argv[6];
    port = atoi(portno);
  }
  
  do {
    // prompt for user input and tokenize the input
    printf("goblin-shell > ");
    cmd = getCommand();
    token = parseCommand(cmd);

    // check to see if user wants help or exit
    if (!strcmp(token[0], "exit") || !strcmp(token[0], "lo") || !strcmp(token[0], "quit") || !strcmp(token[0], "shutdown")) {
      printf("Goodbye! \n");
      printImage();
      free(cmd);
      free(token);
      exit(0);
    } else if (!strcmp(token[0], "help") || !strcmp(token[0], "h")) {
      // display some helpful information
      printf("usage: goblin-shell [COMMANDS] \n\n");
      printf("commands: \n");
      printf("\tcompile [code-files] [link-files] [flags]\t  compile a file on the remote host \n");
      printf("\trun [program-name] [args]\t                  run a program on the remote host \n");
      printf("\thelp\t                                          display usage information \n");
      printf("\texit\t                                          exit the shell \n\n");
    } else if (!strcmp(token[0], "compile")) {
      // create argument list to send to client
      char** command = malloc(sizeof(char*)*OFFSET*TOKEN_BUFSIZE);
      command[0] = strdup("client");
      command[1] = strdup(username);
      command[2] = strdup(servername);
      command[3] = strdup(portno);
      memcpy(command+OFFSET, token, sizeof(char*)*TOKEN_BUFSIZE);

      // create a child to exec the command
      pid_t child = fork();
      if(child > 0) { // parent
        wait(&status);
        free(command);
      } else if (child == 0) { //child
        execv("./client", command);
        perror("Failed to exec. Type help for more information on usage");
        exit(0);
      } else perror("failed to fork");
    } else if (!strcmp(token[0], "run")){
        // create argument list to send to client
        char** command = malloc(sizeof(char*)*OFFSET*TOKEN_BUFSIZE);
        command[0] = strdup("client");
        command[1] = strdup(username);
        command[2] = strdup(servername);
        command[3] = strdup(portno);
        memcpy(command+OFFSET, token, sizeof(char*)*TOKEN_BUFSIZE);
        
        // create a child to exec the command
        pid_t child = fork();
        if(child > 0) { // parent
          wait(&status);
          free(command);
        } else if (child == 0) { //child
          execv("./client", command);
          perror("Failed to exec. Type help for more information on usage");
          exit(0);
        } else perror("failed to fork");
    } else printf("Invalid command. Please try again. Type \"help\" for more information.\n");
    
    free(cmd);
    free(token);
  } while (1);

  return EXIT_SUCCESS;
}

// retrieves the line of the user input
char* getCommand() {
  char* cmd = NULL;
  size_t size = 0;
  getline(&cmd, &size, stdin);
  return cmd;
}

// tokenizes the input from the entire command line
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

// prints the image of the goblin to the console
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
