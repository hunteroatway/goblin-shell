#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 512
#define TOKEN_BUFSIZE 64 

int main(int argc, char* argv[]) {
  struct sockaddr_in sock_addr;
  struct hostent *host;
  socklen_t sock_len;
  int sock, sock_fd, port; 

  int num_char = 512;
  char ch[512];
  char message[BUFSIZE];  // used to send message to server process
  char scpFiles[BUFSIZE]; // used to scp the files
  char* word;

  // is given in form ./client $username $serverName $Port compile $[compliation] 
  char portnum[20];
  char hostname[20];
  char username[50];
  char task[20];

  // make sure number of arguments is correct
  if(argc < 5) {
    printf("Too few arguments. Type help for more information");
  }

  strcpy(username, argv[1]);
  strcpy(hostname, argv[2]);
  strcpy(portnum, argv[3]);
  strcpy(task, argv[4]);
 
  // prepare the message to be sent to server
  if (!strcmp(task , "compile")) {
    int i = 5;
    strcpy(message, task);
    strcat(message, " ");
    strcpy(scpFiles, "goblin.txt");
    strcat(scpFiles, " ");
    while(argv[i] != NULL){      
      if(argv[i][0] == '-') {
        strcat(message, argv[i]);
        strcat(message, " ");
      } else {
        // split the word via a / to get the scp path and to send the file name to server
        word = strrchr(argv[i], '/');
        if (word != NULL) {
          strcat(message, word+1);
        } else {
          strcat(message, argv[i]);
        }
        strcat(scpFiles, argv[i]);
        strcat(scpFiles, " ");
        strcat(message, " ");
      }
      i++;
    }
  } else if (!strcmp(task , "run")) {
    int i = 5;
    strcpy(message, task);
    strcat(message, " ");
    while(argv[i] != NULL){
      strcat(message, argv[i]);
      strcat(message, " ");
      i++;
    }
  } else {
    printf("You have not send a valid command to client. Type \"help\" in the shell for more information. \n");
    return 0;
  }

	// zero out sock_addr struct
	bzero((char *)&sock_addr, sizeof(sock_addr));
	
	// host and port information
	sock_addr.sin_family = AF_INET;
	host = gethostbyname(hostname);
	port = atoi(portnum);
	sock_addr.sin_port = htons(port);

  // copy client information into socket data structure
  memcpy(&sock_addr.sin_addr, host->h_addr, host->h_length);

  // create socket file descriptor
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket failed");
    exit(1);
  }
  
  // connect to socket 
  if ((connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr))) != 0) {
    perror("connect failed");
    exit(1);
  }
  
  char dir[BUFSIZE];
  int dir_size = read(sock, dir, BUFSIZE);
  fflush(stdout);
  printf("%s", dir);

   if (!strcmp(task , "compile")) {

     // path for the scp. combine username, server and path
    char scpPath[512];
    strcpy(scpPath, username);
    strcat(scpPath, "@");
    strcat(scpPath, hostname);
    strcat(scpPath,":");
    strcat(scpPath, dir);
    strcat(scpFiles, scpPath);
    printf("the SCPpath is %s \n", scpFiles);

    // move a file over
    pid_t child = fork();
    int status;
    if(child == -1){
      perror("fork");
      exit(0);
    } else if (child == 0) {
      execl("/usr/bin/scp", "scp", scpFiles, NULL);
      perror("exec failure");
      exit(1);

    } else {
      // wait for files to copy
      wait(&status);
      // send message to server
      write(sock, message, strlen(message));
    }
   } else if (!strcmp(task , "run")){
      // send message to server
      write(sock, message, strlen(message));
   } else {
     printf("Error\n");
     return 0;
   }
 
/*  while((num_char=read(sock, ch, 512)) > 0) {
    if (write(1, ch, num_char) < num_char) {
      perror("write failed");
      exit(1);
    }*/
    close(sock);
  }

	return EXIT_SUCCESS;
 // }

  return EXIT_SUCCESS;
}
