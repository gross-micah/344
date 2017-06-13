#include <assert.h>
#include <fcntl.h>
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
  char *s = "START_ROOM";
  char *m = "MID_ROOM";
  char *e = "END_ROOM";

  struct Room gameboard[7];
  int i, j, k, random;
  int taken[10];
  //initialize array to ensure no repeated names
  for (i = 0; i < 10; i++)
  {
    taken[i] = 0;
  }
  //initialize connectionsboard since I was getting garbage values on flip. fine locally
  for (i = 0; i < 7; i++)
  {
    for (j = 0; j < 7; j++)
    {
      gameboard[i].connections[j] = 0;
    }
  }



  //populate each room name and set count to 0
  for (i = 0; i < 7; i++)
  {
    //ensure no garbage in the arrays for values and names
    //memset(gameboard[i].roomName, '\0', 100);

    gameboard[i].connectionsCount = 0;
    random = rand() % 10;
    if (taken[random] == 0)
    {
      gameboard[i].roomName = rooms[random];
      taken[random] = 1;
    }
    else
    {
      i--;
    }
  }
  //set first room as start
  gameboard[0].myType = start;
  //set 2nd room as end
  gameboard[1].myType = end;
  for (i = 2; i < 7; i++)
  {
    gameboard[i].myType = mid;
  }

  //create connections between 3 and 6
  for (i = 0; i <= 6; i++)
  {
    for (j = 0; j < 3; j++)
    {
      random = rand() % 7;
      //can't connect to yourself and connection must be available
      if (random != i && gameboard[i].connections[random] == 0)
      {
        //confirm the room still needs more connections
        if (gameboard[i].connectionsCount < 6)
        {
          //confirm the randomly picked room is available
          if (gameboard[random].connectionsCount < 6)
          {
            gameboard[i].connections[random] = 1;
            gameboard[random].connections[i] = 1;
            gameboard[i].connectionsCount++;
            gameboard[random].connectionsCount++;
          }
          //if connection can't be reached, roll back j to redo loop again.
          else {
            j--;
          }

        }
        //room i already has  too many connections. set j to move to next room i
        else {
          j = 3; // !
        }
      }
      else if (gameboard[i].connectionsCount >= 3) {
       j = 3;
      }
      //initial random value was already taken or was the room i itself
      else {
        j--;
      }
    }
  }

  //was getting junk values for final room, so pulling out and building on its own
  gameboard[6].connections[6] = 0;
  for (i = 0; i < 6; i++)
  {
      gameboard[6].connections[i] = gameboard[i].connections[6];
      if (gameboard[6].connections[i] == 1) gameboard[6].connectionsCount++;
  }

  //all rooms have now been completely initialized and connected

  //grab processor ID for folder
  int pid = getpid();
  //initialize folder name and append pid. this directory will be populated by rooms
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

  char fileName[100];
  char buffer[100];
  char name[15];
  FILE* fp;

  //construct the 7 files
  for (i = 0; i < 7; i++)
  {
    memset(fileName, '\0', 100);
    memset(buffer, '\0', 100);
    memset(name, '\0', 100);
    sprintf(fileName, "./grossmmi.rooms.%d/%s", (int)pid, gameboard[i].roomName);
    fp = fopen(fileName, "w+");
    sprintf(buffer, "ROOM NAME: %s\n", gameboard[i].roomName);
    fputs(buffer, fp);
    //while room file is open, write necessary lines to it
    for (j = 0; j < 7; j++)
    {
      if(j != i && gameboard[i].connections[j] == 1)
      {
        sprintf(buffer, "CONNECTION %d: %s\n", j + 1, gameboard[j].roomName);
        fputs(buffer, fp);
      }
    }
    //identify room type
    if (gameboard[i].myType == start)
    {
      sprintf(buffer, "ROOM TYPE: %s\n", s);
    }
    else if (gameboard[i].myType == mid)
    {
      sprintf(buffer, "ROOM TYPE: %s\n", m);
    }
    else
    {
          sprintf(buffer, "ROOM TYPE: %s\n", e);
    }
    fputs(buffer, fp);
    fclose(fp);
  }


  return 0;
}
