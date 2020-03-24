#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[]) {
	struct sockaddr_in sock_addr;
  struct hostent *host;
  socklen_t sock_len;
  int sock, sock_fd, port; 


  int num_char = 512;
  char ch[512];

  
  char portnum[20];
  char hostname[20];
  printf("\n hostname: ");
  scanf("%s", hostname);
 
  printf("\n port: ");
  scanf("%s", portnum);
 
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
  

 
  while((num_char=read(sock, ch, 512)) > 0) {
    if (write(1, ch, num_char) < num_char) {
      perror("write failed");
      exit(1);
    }
    close(sock);
  }



	return EXIT_SUCCESS;
}
