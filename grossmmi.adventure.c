#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


enum type {start, mid, end};

struct Room {
  char roomName[100];
  int connections[7];
  int connectionsCount;
  enum type myType;
};

int main()
{
  struct Room gameboard[7];
  time_t current;
  time_t recent = -1;
  size_t size = 0;
  //begin and win used to keep track of array index for start end rooms
  int i, j, k, begin, win, done, found;
  int index = 0;
  char line[100];
  char folder[250];
  char fullpath[100];
  char buffer[100];
  char *lineEntered = NULL;
  int path[20];
  FILE *fp;
  //initialize the gameboard value for each room
  for (i = 0; i < 7; i++)
  {
    gameboard[i].connectionsCount = 0;
    memset(gameboard[i].roomName, '\0', 100);
  }

  //get directory of most recently touched game folder
  DIR *directory;
  struct stat dStats;
  struct dirent *lineItem;
  memset(fullpath, '\0', 100);
  getcwd(fullpath, 100);
  directory = opendir(fullpath);
  if (directory == NULL)
  {
    printf("Error opening directory.");

    exit(1);
  }
  /*continue looping to find most recently added game directory
  Most recent will be saved in folder variable*/
  char *roomsFolder = "grossmmi.rooms.";
  while ((lineItem = readdir(directory)))
  {
    if (strstr(roomsFolder, lineItem->d_name) != NULL)
    {
      printf("it's getting into the if statement");
      stat(lineItem->d_name, &dStats);
      current = dStats.st_mtime;
      if (current > recent)
      {
        memset(folder, '\0', sizeof(folder));
        recent = current;
        strcpy(folder, lineItem->d_name);
      }
    }
  }
  //move to new directory. Throw error if problem.
  printf("value for folder is %s", folder);
  directory = opendir(folder);
  if(directory == NULL)
  {
    printf("Error opening up game directory\n");
  }

  //open files 1 by 1 and populate gameboard with room details
  //when finding the room with start and end type, capture those values
  //note: can't hard code a for loop. need to watch out for current and
  //working directories
  index = 0
  while ((lineItem = readdir(directory)) != NULL)
  {
    //only act on items known be room names
    if (strlen(lineItem->d_name) > 3)
    {
      memset(line, '\0', sizeof(line));
      memset(fullpath, '\0', sizeof(fullpath));
      sprintf(fullpath, "./%s/%s", folder, lineItem->d_name);
      fp = fopen(fullpath, "r");
      if (fp == NULL)
      {
        printf("Error opening files.");
        exit(1);
      }

      //while loop to continue processing the contents of the file
      while (fgets(line, sizeof(line), fp))
      {
        if (strstr(line, "ROOM NAME") != NULL)
        {
          //find the character before the newline
          int last;
          for (j = 0; j < 100; j++)
          {
            if (line[j] == '\n')
            {
              last = j - 1;
              j = 100;
            }
          }
          //note: line + 11 is the size of "ROOM NAME: "
          strncpy(gameboard[i].roomName, line + 11, last - 11);
        }
        //set any connections
        else if (strstr(line, "CONNECTION") != NULL)
        {
          //we know from formatting that the 11th character in the line is the number
          int contact = line[11] - '0';
          if (gameboard[i].connections[contact] == 0)
          {
            gameboard[i].connections[contact] = 1;
            gameboard[i].connectionsCount += 1;
            gameboard[contact].connections[i] = 1;
            gameboard[contact].connectionsCount += 1;
          }
        }
        //set room type
        else if (strstr(line, "ROOM TYPE") != NULL)
        {
          if (strstr(line, "END") != NULL)
          {
            gameboard[i].myType = end;
            win = i;
          }
          else if (strstr(line, "START") != NULL)
          {
            gameboard[i].myType = start;
            begin = i;
          }
          else
            gameboard[i].myType = mid;
        }
      }
    }
    fclose(fp);
  }
  closedir(directory);

  //game board established with rooms created. can begin playing the gameboard
  done = 0;
  int steps = 0;
  int location = begin;
  int period = 0;
  do {
    printf("CURRENT LOCATION: %s\n", gameboard[location].roomName);
    printf("POSSIBLE CONNECTIONS: ");
    for (j = 0; j < 7; j++)
    {
      if (gameboard[i].connections[j] == 1 && j != location)
      {
        printf("%s", gameboard[j].roomName);
        period += 1;
      }
      if (period == gameboard[i].connectionsCount)
        printf(".");
      else
        printf(", ");
    }
    printf("\nWHERE TO? >");
    getline(&lineEntered, &size, stdin);
    if (strcmp(buffer, "time") == 0)
    {
      //return to add functioncality for pthreads
    }
    for (i = 0; i < 7; i++)
    {
      found = 0;
      if (strcmp(buffer, gameboard[i].roomName) == 0)
      {
        found = 1;
        if (gameboard[location].connections[i] == 1 && i != location)
        {
          location = i;
          steps += 1;
        }
        else
        {
          printf("INVALID CONNECTION\n");
        }
      }
    }
    if (found == 0)
      printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
    if (gameboard[location].myType == end)
      done = 1;
    free(lineEntered);
    lineEntered = NULL;
  } while(done != 1);
  printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!");
  printf("\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
  for (i = 0; i <= steps; i++)
  {
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%s", gameboard[path[i]].roomName);
    printf("%s\n", buffer);
  }

  return 0;
}
