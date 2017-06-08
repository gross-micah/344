#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  //check for proper argument count and formatting
  if (argc < 2)
  {
    printf("syntax: otp_enc_d <listening_port>\n");
    exit(1);
  }

  int portNumber = atoi(argv[1]);
  if (portNumber > 65535 || portNumber < 0)
  {
    printf("Error: invalid port number\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  struct sockaddr_in serverAddress;
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  struct hostent* serverHostInfo;
  char buffer[256];
  serverAddress.sin_family = AF_UNIX;
  serverAddress.sin_port = htons(portNumber);
  serverHostInfo = gethostbyname(localhost);
  if (serverHostInfo == NULL)
  {
    fprintf(stderr, "Error (client): No such host\n");
    exit(0)
  }

  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

  //establish socket
  int socketFD = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socketFD == -1)
  {
    printf("Error: failed to create socket\n");
    exit(1);
  }

  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
  {
    printf("Error: failed to connect\n");
    exit(1);
  }

  


  return 0;
}
