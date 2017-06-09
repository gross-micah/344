#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv)
{
  srand(time(NULL));
  //check for proper argument count and formatting
  if (argc < 2)
  {
    printf("syntax: otp_enc_d <listening_port>\n");
    exit(1);
  }

  int keyLength = atoi(argv[1]);
  if (keyLength == 0)
  {
    perror("Error: invalid keylength value\n");
    exit(1);
  }

  //define variables
  int charsWritten;
  int charsRead;
  int i;
  int random;
  char key[256];
  char letter;
  memset(buffer, '\0', 256);
  //write random valid characters to key
  //rand number between 65 and 91. 91 == [space]
  for (i = 0; i < keyLength; i++)
  {
    random = rand() % (28) + 65;
    if (random == 91) random = 32;
    letter = (char) random;
    key[i] = letter;
  }
  //make sure to end with newline character
  key[keyLength] = '\n';

  //print to standard out
  printf("%s", keygen);

  return 0;
}
