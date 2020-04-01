#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 75

int main(int argc, char* argv[]) {
	struct sockaddr_in sock_addr;
  struct hostent *host;
  socklen_t sock_len;
  int sock, sock_fd, port; 


  int num_char = 512;
  char ch[512];

  // is given in form ./client $username $serverName $Port compile $[compliation] 
  char portnum[20];
  char hostname[20];
  char username[50];
  char task[20];
 /*  printf("\n hostname: ");
  scanf("%s", hostname);
 
  printf("\n port: ");
  scanf("%s", portnum); */
  strcpy(username, argv[1]);
  strcpy(hostname, argv[2]);
  strcpy(portnum, argv[3]);
  strcpy(task, argv[4]);
  return 0;
 
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
  printf("the path is %s \n", dir);

  // path for the scp. combine username, server and path
  char scpPath[512];
  strcpy(scpPath, username);
  strcat(scpPath, "@");
  strcat(scpPath, hostname);
  strcat(scpPath,":");
  strcat(scpPath, dir);
  printf("the SCPpath is %s \n", scpPath);

 // try to move a file over
  pid_t child = fork();
  int status;
  if(child == -1){
    perror("fork");
    exit(0);
  } else if (child == 0) {
    execl("/usr/bin/scp", "scp", "test/test.txt", scpPath, NULL);
    perror("exec failure");
    exit(1);

  } else {
    wait(&status);
    printf("Maybe it worked?");

  } 
 
  while((num_char=read(sock, ch, 512)) > 0) {
    if (write(1, ch, num_char) < num_char) {
      perror("write failed");
      exit(1);
    }
    close(sock);
  }



	return EXIT_SUCCESS;
}
