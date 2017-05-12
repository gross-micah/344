#include <unistd.h>
#include <stdlib.h>

int main()
{
  //seed random
  srand(time(NULL));
  //grab processor ID for folder
  int pid = getpid();
  //initialize folder name and append pid
  char directory[100];
  memset(directory, '\0', 100);
  directory = "grossmmi.rooms.";
  strcat(directory, itoa(pid));
  mkdir(directory, 0777);
  //create 7 room files
  int rooms = 7;
  int i;
  char fileName[100];
  for (i = 1; i <= rooms; i++)
  {
    memset(fileName, '\0', 100);
    fileName = "/";
    strcat(fileName, directory);
    strcat(fileName, "/room");
    strcat(fileName, itoa(i));
    FILE *fp
  }

  return 0;
}
