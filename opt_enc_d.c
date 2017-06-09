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
    perror("Error: invalid port number\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  int pid;
  int listenSocketFD, establishedConnectionFD;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo;
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  char buffer[256];
  serverAddress.sin_family = AF_UNIX;
  serverAddress.sin_port = htons(portNumber);

  //establish socket
  listenSocketFD = socket(AF_UNIX, SOCK_STREAM, 0);
  if (listenSocketFD < 0)
  {
    perror("Error: failed to create socket\n");
    exit(1);
  }
  //setup listening on socket
  if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
  {
    perror("Error: binding failed\n");
  }
  //begin listening
  if(listen(listenSocketFD, 5) < 0)
  {
    perror("Error: listening failed\n");
  }
  sizeOfClientInfo = sizeof(clientAddress);
  //infinite loop to listen for signals.
  while (1)
  {

    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    //check if connection accepted
    if (establishedConnectionFD < 0)
    {
      perror("Error: failed to accept\n");
    }
    //manage forking and activity for child signal before terminating process
    pid = fork();

    if (pid < 0)
    {
      perror("Error: failure at fork\n");
    }

    //do encryption here for child process
    else if (pid == 0)
    {
      memset(buffer, '\0', 256);
      charsRead = recv(establishedConnectionFD, buffer, 255, 0);
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

      


    }

    //send it back here

    close(establishedConnectionFD);
  }
  close(listenSocketFD);

  return 0;
}
