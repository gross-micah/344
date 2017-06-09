#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFERSIZE = 100000

int main(int argc, char** argv)
{
  //check for proper argument count and formatting
  if (argc < 4)
  {
    printf("syntax: otp_enc <plaintext> <key> <port>\n");
    exit(1);
  }

  int portNumber = atoi(argv[1]);
  if (portNumber > 65535 || portNumber <= 0)
  {
    perror("Error: invalid port number\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  int i;
  int confirmation;
  int socketFD;
  struct hostent* serverHostInfo;
  struct sockaddr_in serverAddress;
  int fileDescriptor;
  socklen_t sizeOfClientInfo;
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  //use very large buffer to not need to loop for capture.
  char bufferPlain[BUFFERSIZE];
  char bufferKey[BUFFERSIZE];
  char confirm[256];

  fileDescriptor = open(argv[1], O_RDONLY);
  if (fileDescriptor < 0)
  {
    perror("Error: failed to open plaintext\n");
    exit(1);
  }

  memset(bufferPlain, '\0', 256);
  charsRead = recv(fileDescriptor, bufferPlain, BUFFERSIZE, 0);
  if (charsRead < 0)
  {
    perror("Error: reading from socket\n");
    exit(1);
  }
  //enter into loop until all bytes are captured.
  /* note: increased buffer size to avoid running through loop issues
  while (charsRead > 0)
  {

    memset(bufferPlain, '\0', 256);
    charsRead = recv(fileDescriptor, bufferPlain, 255, 0);
  }
  */
  close(fileDescriptor)

  //now open and process the key file
  fileDescriptor = open(argv[2], O_RDONLY);
  if (fileDescriptor < 0)
  {
    perror("Error: failed to open plaintext\n");
    exit(1);
  }
  memset(bufferKey, '\0', BUFFERSIZE);
  //save charsRead value for plaintext for comparison
  int comparePlain = charsRead;

  charsRead = recv(fileDescriptor, bufferKey, BUFFERSIZE, 0);
  if (charsRead < 0)
  {
    perror("Error: reading from socket\n");
    exit(1);
  }
  //exit if key is too small
  if (charsRead < comparePlain)
  {
    perror("Error: key must be as large as message")
    exit(1);
  }

  //verify only capital letters or [space]
  for (i = 0; i < charsRead - 1; i++)
  {
    if (bufferKey[i] < 'A' && bufferKey[i] != ' ')
    {
      perror("otp_enc error: input contains bad characters\n");
      exit(1);
    }
    else if (bufferKey[i] > 'Z')
    {
      perror("otp_enc error: input contains bad characters\n");
      exit(1);
    }
  }

  //attach signal that opt_enc is sending the message, not opt_dec
  if (bufferKey[charsRead - 1] == '\n')
  {
    bufferKey[charsRead - 1] = 'E';
    bufferKey[charsRead] = 'N';
    bufferKey[charsRead + 1] = 'C';
    bufferKey[charsRead + 2] = '\n';
  }

  serverAddress.sin_family = AF_UNIX;
  serverAddress.sin_port = htons(portNumber);
  serverHostInfo = gethostbyname("localhost");
  if (serverHostInfo == NULL)
  {
    fprintf(stderr, "Error (client): No such host\n");
    exit(0);
  }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

  //setup socket
  socketFD = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socketFD < 0)
  {
    perror("Error: failed to create socket\n");
    exit(1);
  }

  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
  {
    printf("Error: failed to connect\n");
    exit(2);
  }

  //send plaintext
  charsWritten = send(socketFD, bufferPlain, strlen(bufferPlain), 0);
  if (charsWritten < 0)
  {
    perror("Error: writing to socket\n");
    exit(2);
  }
  if (charsWritten < strlen(bufferPlain))
  {
    perror("Error: not all data written to socket\n");
    exit(2);
  }
  //confirm receipt from server / opt_enc_d
  confirmation = recv(socketFD, confirm, 255, 0);
  if (confirmation < 0)
  {
    perror("Error: no confirmation after plaintext transmission\n")
    exit(2);
  }

  //send key
  charsWritten = send(socketFD, bufferKey, strlen(bufferKey), 0);
  if (charsWritten < 0)
  {
    perror("Error: writing to socket\n");
    exit(2);
  }
  if (charsWritten < strlen(bufferKey))
  {
    printf("Error: not all data written to socket\n");
    exit(2);
  }
  //confirm receipt from server / opt_enc_d
  confirmation = recv(socketFD, confirm, 255, 0);
  if (confirmation < 0)
  {
    perror("Error: no confirmation after plaintext transmission\n")
    exit(2);
  }

  return 0;
}
