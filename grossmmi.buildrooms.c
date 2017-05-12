#include <unistd.h>
#include <stdlib.h>

enum type = {start, mid, end};

struct Room {
  char roomName[100];
  int connections[7];
  int position;
  int connectionsCount;
  enum type myType;
};



int main()
{
  //setup room structures
  char *rooms[10] = {"bathroom", "kitchen", "Chipotle", "bedroom", "office",
  "solarium", "garage", "library", "moat", "basement"};

  const char *typeString[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"}

  struct Room gameboard[10];

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
  int i, j, k;
  int write;
  char fileName[100];
  //string for proper formatting into file
  char *name = "ROOM NAME: ";
  char *conect = "CONNECTION ";
  char *roomType = "ROOM TYPE: ";
  char *colon = ": ";
  char *nline = "\n";

  //construct the 7 files
  for (i = 0; i < 7; i++)
  {
    memset(fileName, '\0', 100);
    fileName = "/";
    strcat(fileName, directory);
    strcat(fileName, "/room");
    strcat(fileName, itoa(i));
    strcat(fileName, ".txt");
    FILE *fp = fopen(fileName, "w+");
    write = fwrite(name, sizeof(char), sizeof(name), fp);
    write = fwrite(gameboard[i]->roomName, sizeof(char), sizeof(gameboard[i]->roomName), fp);
    write = fwrite(nline, sizeof(char), sizeof(nline));

    //loop through array of connections. AND statement sets it to ignore itself in connections
    int count = 1;
    for (j = 0; j < 7; j++)
    {
      if (gameboard[i]->connections[j] == 1 && i != j)
      {
        write = fwrite(connect, sizeof(char), sizeof(connect));
        char num = itoa(count);
        write = fwrite(num, sizeof(char), sizeof(num));
        write = fwrite(colon, sizeof(char), sizeof(colon));
        
      }
    }

    fclose(fp);
  }

  return 0;
}
