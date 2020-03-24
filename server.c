#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 0

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
      
    int file = open("test.file", O_RDONLY);
    if(file == -1)
      write(sock_fd, "No information, dude.\n", strlen("No information, dude.\n"));
    else
    {
      while((num_char = read(file, ch, 512)) > 0)
        if (write(sock_fd, ch, num_char) < num_char) {
          perror("writing failed");
          exit(1);
        }
      close(file);
    }
    close(sock_fd);
  }

  return EXIT_SUCCESS;
}
