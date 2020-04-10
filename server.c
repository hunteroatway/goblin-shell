#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 0
#define BUFSIZE 512
#define TOKEN_BUFSIZE 64

char** parse_command(char* _cmd);
 
int main(int argc, char* argv[]) {
  struct sockaddr_in sock_addr;
  struct hostent *host = NULL;
  socklen_t sock_len;
  int sock;
  int sock_fd;
  char hostname[256];

  // zero out sock_addr struct
  bzero((char *)&sock_addr, sizeof(sock_addr));

  // host and port information
  sock_addr.sin_family = AF_INET;
  gethostname(hostname, sizeof(hostname));
  host = gethostbyname(hostname);
  sock_addr.sin_port = htons(PORT);
 
  // copy host information into socket data structure
  if (host != NULL)
    memcpy(&sock_addr.sin_addr, host->h_addr, host->h_length);
  
  // create socket file descriptor
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket failed"); 
    exit(1);
  }

  // bind socket with system
  if (bind(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) != 0) {
    perror("bind failed");
    exit(1);
  }
  
  // print assigned port
  sock_len = sizeof(sock_addr);
  getsockname(sock, (struct sockaddr*)&sock_addr, &sock_len);
  printf("Port: %d\n", ntohs(sock_addr.sin_port));

  // listen for input (1 connection)
  if (listen(sock, 1) != 0) {
    perror("listen failed");
    exit(1);
  }

  int num_char = 512;
  char ch[512];
  for(;;)
  {
    if ((sock_fd = accept(sock, NULL, NULL)) == -1) {
      perror("accept failed");
      exit(1);
    }
    
    // get current directory 
    pid_t child;
    int pfd[2];
    char dir[BUFSIZE];
    int status;

    if(pipe(pfd) == -1) {
      perror("Pipe failue");
      exit(1);
    }

    child = fork();
    if(child == -1){
      perror("fork failed");
      exit(1);
    } else if (child == 0) { // child
      // close read end
      close(pfd[0]);
      // redirect std out to pipe
      dup2(pfd[1], fileno(stdout));

      // exec pwd
      if(execlp("pwd", "pwd", NULL) == -1){
        perror("Error in calling exec!");
        exit(0);
      }
      exit(0);
    } else { //parent
      close(pfd[1]);
      wait(&status);

      int nread = read(pfd[0], dir, BUFSIZE);
      if (nread!=-1){
          dir[nread] = '\0';
          fflush(stdout);
      }

      // send current directory to client
      write(sock_fd, dir, strlen(dir));
    }

      // redirects stdout from server to client
      dup2(sock_fd, fileno(stdout));

      num_char = read(sock_fd, ch, 512);
      char** token = parse_command(ch);

      if (!strcmp(token[0] , "compile")) {
        char outputFile[32];
        char* fileExt = strrchr(token[1], '.');
        char compilerType[8];

        // get the output file name from the supplied code fiel
        int j = 0;
        while (token[1][j] != '.') {
          outputFile[j] = token[1][j];
          j++;
        }

        // determine the compiler based on the file extension
        if (!strcmp(fileExt, ".c")) {
          strcpy(compilerType, "gcc");
        } else if (!strcmp(fileExt, ".cpp")) {
          strcpy(compilerType, "g++");
        } else {
          perror("invalid file type");
          exit(1);
        }

        // fork the process to allow for the exec to run
        pid_t pid = fork();
        if (pid == -1) {
          perror("fork failed");
          exit(1); 
        } else if (pid == 0) {
          // run the file name that was passed via the command with the list of args
          execlp(compilerType, compilerType, token[1], "-o", outputFile, NULL);
          perror("Failed to exec. Type help for more information on usage");
          exit(0);
        } else {
          wait(&status);
        }
      } else if (!strcmp(token[0] , "run")) {
        // create the list of command line arguments
        char** args = malloc(sizeof(char*)*(num_char+1));

        // fork the process to allow for the exec to run
        pid_t pid = fork();
        if (pid == -1) {
          perror("fork failed");
          exit(1); 
        } else if (pid == 0) {
          // run the file name that was passed via the command with the list of args
          execv(token[1], args);
          perror("Failed to exec. Type help for more information on usage");
          exit(0);
        } else {
          wait(&status);
        }
      } else {
        perror("invalid command \n");
        exit(1);
      }

    close(sock_fd);
  }

  return EXIT_SUCCESS;
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