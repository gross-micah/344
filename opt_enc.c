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
  if (argc < 4)
  {
    printf("syntax: otp_enc <plaintext> <key> <port>\n");
    exit(1);
  }

  int portNumber = atoi(argv[1]);
  if (portNumber > 65535 || portNumber < 0)
  {
    perror("Error: invalid port number\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  int pid;
  int listenSocketFD, establishedConnectionFD;
  struct hostent* serverHostInfo;
  struct sockaddr_in serverAddress;
  int fileDescriptor;
  socklen_t sizeOfClientInfo;
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  char buffer[256];
  serverAddress.sin_family = AF_UNIX;
  serverAddress.sin_port = htons(portNumber);


  fileDescriptor = open(argv[1], O_RDONLY);
  if (fileDescriptor < 0)
  {
    perror("Error: failed to open plaintext\n");
    exit(1);
  }

  memset(buffer, '\0', 256);
  charsRead = recv(fileDescriptor, buffer, 255, 0);
  if (charsRead < 0)
  {
    perror("Error: reading from socket\n");
  }
  //check 1st 3 digits in buffer to verify signal from enc not dec
  int confirmed = 0;
  if(buffer[0] != 'E') confirmed = -1;
  if(buffer[1] != 'N') confirmed = -1;
  if(buffer[2] != 'C') confirmed = -1;
  if (confirmed == -1)
  {
    perror("Error: incorrect program identifier\n");
  }

  "Error: could not contact otp_enc_d on port"
  "Error: key ‘" myshortkey "’ is too short"
  "otp_enc error: input contains bad characters"



  if (connect(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
  {
    printf("Error: failed to connect\n");
    exit(1);
  }

  serverHostInfo = gethostbyname(localhost);
  if (serverHostInfo == NULL)
  {
    fprintf(stderr, "Error (client): No such host\n");
    exit(0)
  }

  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

  return 0;
}
