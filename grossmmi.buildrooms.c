#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

enum type {start, mid, end};

struct Room {
  char *roomName;
  int connections[7];
  int position;
  int connectionsCount;
  enum type myType;
};

int main()
{
  //seed random
  srand(time(NULL));
  //setup room structures
  char *rooms[10] = {"bathroom", "kitchen", "Chipotle", "bedroom", "office",
  "solarium", "garage", "library", "moat", "basement"};
  const char *typeString[3] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

  struct Room gameboard[7];
  int i, j, k;
  int taken[10];
  //initialize array to ensure no repeated names
  for (i = 0; i < 10; i++)
  {
    taken[i] = 0;
  }
  //initialize the position value for each room
  for (i = 0; i < 7; i++)
  {
    gameboard[i].position = i;
  }

  //populate each room
  for (i = 0; i < 7; i++)
  {
    //ensure no garbage in the arrays for values and names
    //memset(gameboard[i].roomName, '\0', 100);
    memset(gameboard[i].connections, 0, 7);
    int done = 0;
    int temp;
    do {
      temp = rand() % 10;
      //take name from rooms array if available

      if (taken[temp] == 0)
      {
        gameboard[i].roomName = rooms[temp];
        taken[temp] = 1;
        done = 1;
      }
    } while(done != 1);

    if (i == 0)
    {
      gameboard[i].myType = start;
    }
    else if (i == 1)
    {
      gameboard[i].myType = end;
    }
    else
    {
      gameboard[i].myType = mid;
    }

    done = 0;
    //create connections between 3 and 6
    int rdm1 = rand() % 3 + 4;
    do {
      int rdm2 = rand() % 7;
      if (gameboard[i].connections[rdm2] == 0)
      {
        //every connection needs it reflected in the room being connected
        gameboard[i].connections[rdm2] = 1;
        gameboard[i].connectionsCount += 1;
        gameboard[rdm2].connections[i] = 1;
        gameboard[rdm2].connectionsCount += 1;
      }
      if (gameboard[i].connectionsCount >= rdm1)
        done = 1;

    } while(done != 1);

  }
  //all rooms have now been completely initialized and connected

  //grab processor ID for folder
  int pid = getpid();
  //initialize folder name and append pid
  char directory[100];
  memset(directory, '\0', 100);
  strcpy(directory, "grossmmi.rooms.");
  char input[10];
  for (i = 0; i < 10; i++)
  {
    input[i] = '\n';
  }
  i = sprintf(input, "%d", pid);
  strcat(directory, input);
  mkdir(directory, 0777);



  //create 7 room files
  int write;
  char fileName[100];
  //string for proper formatting into file
  char *name = "ROOM NAME: ";
  char *connect = "CONNECTION ";
  char *roomType = "ROOM TYPE: ";
  char *colon = ": ";
  char *nline = "\n";

  //construct the 7 files
  for (i = 0; i < 7; i++)
  {
    memset(fileName, '\0', 100);
    strcpy(fileName, "/");
    strcat(fileName, directory);
    strcat(fileName, "/room");
    char roomNo[2];
    for (j = 0; j < 2; j++)
    {
      roomNo[i] = '\0';
    }
    j = sprintf(roomNo, "%d", i);
    strcat(fileName, roomNo);
    strcat(fileName, ".txt");
    FILE *fp = fopen(fileName, "w+");

    write = fwrite(name, sizeof(char), sizeof(name), fp);
    write = fwrite(gameboard[i].roomName, sizeof(char), sizeof(gameboard[i].roomName), fp);
    write = fwrite(nline, sizeof(char), sizeof(nline), fp);

    //loop through array of connections. AND statement sets it to ignore itself in connections
    //note: this is nested. The room we care about is of value i. the value j is for its connection.
    int count = 1;
    for (j = 0; j < 7; j++)
    {
      if (gameboard[i].connections[j] == 1 && i != j)
      {
        write = fwrite(connect, sizeof(char), sizeof(connect), fp);
        char num[2];
        for (j = 0; j < 2; j++)
        {
          num[j] = '\0';
        }
        k = sprintf(num, "%d", i);
        count++;
        write = fwrite(num, sizeof(char), sizeof(num), fp);
        write = fwrite(colon, sizeof(char), sizeof(colon), fp);
        write = fwrite(gameboard[j].roomName, sizeof(char), sizeof(gameboard[j].roomName), fp);
        write = fwrite(nline, sizeof(char), sizeof(nline), fp);
      }
    }
    int thisType;
    if (gameboard[i].myType == start)
      thisType = 0;
    else if (gameboard[i].myType == mid)
      thisType = 1;
    else
      thisType = 2;

    //write final line of room type
    write = fwrite(roomType, sizeof(char), sizeof(roomType), fp);
    write = fwrite(typeString[thisType], sizeof(char), sizeof(typeString[thisType]), fp);
    write = fwrite(nline, sizeof(char), sizeof(nline), fp);

    //close the file
    fclose(fp);
  }

  return 0;
}
