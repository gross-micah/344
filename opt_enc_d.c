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
  if (portNumber > 65535 || portNumber <= 0)
  {
    perror("Error: invalid port number\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  int pid;
  int i;
  int BUFFERSIZE = 100000;
  int listenSocketFD, establishedConnectionFD;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo;
  memset((char*)&serverAddress, '\0', sizeof(serverAddress));
  char buffer[256];
  int cypherLength;
  char cypher[BUFFERSIZE];
  char key[BUFFERSIZE];
  serverAddress.sin_family = AF_UNIX;
  serverAddress.sin_port = htons(portNumber);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
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
      int index = 0;
      int endChunk;
      memset(buffer, '\0', 256);
      do
      {
        charsRead = recv(establishedConnectionFD, buffer, 255, 0);
        endChunk = index + charsRead;
        for (i = index; i < endChunk; i++)
        {
          //check for incoming bad characters
          if (buffer[i] < 'A' && buffer[i] != ' ')
          {
            perror("otp_enc_d error: input contains bad characters\n");
            exit(1);
          }
          else if (key[i] > 'Z')
          {
            perror("otp_enc_d error: input contains bad characters\n");
            exit(1);
          }
          else if (buffer[i] == ' ')
            cypher[i] = '[';
          else
            cypher[i] = buffer[i];
          index++;
        }
        memset(buffer, '\0', 256);
      }while (charsRead > 0);

      cypherLength = index;

      if (charsRead < 0)
      {
        perror("Error: reading from socket\n");
      }
      //check 1st 3 digits in buffer to verify signal from enc not dec
      int confirmed = 0;
      if(cypher[0] != 'E') confirmed = -1;
      if(cypher[1] != 'N') confirmed = -1;
      if(cypher[2] != 'C') confirmed = -1;
      if (confirmed == -1)
      {
        perror("Error: incorrect program identifier\n");
        exit(1);
      }

      buffer[0] = 'O';
      buffer[1] = 'K';
      //send an 'ok' to continue the process
      charsWritten = send(establishedConnectionFD, buffer, 2, 0);
      if (charsWritten < 2)
      {
        perror("opt_enc_d error: confirmation signal after message receipt failed\n");
      }
      //capture the key value
      memset(buffer, '\0', 256);
      memset(key, '\0', BUFFERSIZE);
      index = 0;
      do
      {
        charsRead = recv(establishedConnectionFD, buffer, 255, 0);
        endChunk = index + charsRead;
        for (i = index; i < endChunk; i++)
        {
          if (buffer[i] == ' ')
            key[i] = '[';
          else
            key[i] = buffer[i];
          index++;
        }
        memset(buffer, '\0', 256);
      } while (charsRead > 0);
      if (charsRead < 0)
      {
        perror("Error: reading from socket for key\n");
      }
      //convert values
      int temp;
      int keyInt;
      char value;
      for (i = 0; i < cypherLength; i++)
      {
        temp = (int) cypher[i] - 64;
        keyInt = key[i] - 64;
        value = (char) ((temp + keyInt) % 27 + 64);
        cypher[i] = value;
        if (cypher[i] == '@')
          cypher[i] = ' ';
      }

      //send it back here
      charsWritten = send(establishedConnectionFD, cypher, cypherLength, 0);
      if (charsWritten < 2)
      {
        perror("opt_enc_d error: confirmation signal after message receipt failed\n");
      }
      //cleanup and exit
      close(establishedConnectionFD);
      exit(0);
    }
    //parent process
    else
    {

      close(establishedConnectionFD);
    }

  }
  close(listenSocketFD);

  return 0;
}
