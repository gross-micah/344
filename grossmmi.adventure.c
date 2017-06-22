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
  char *rooms[10] = {"bathroom", "kitchen", "Chipotle", "bedroom", "office",
  "solarium", "garage", "library", "moat", "basement"};
  //begin and win used to keep track of array index for start end rooms
  int i, j, k, begin, win, done, found;
  int index = 0;
  char line[100];
  char folder[256];
  memset(folder, '\0', sizeof(folder));
  char fullpath[256];
  char filesFolder[256];
  memset(filesFolder, '\0', sizeof(filesFolder));
  char buffer[100];
  char *lineEntered = NULL;
  char *newLine = NULL;
  int path[20];
  char trail[300];
  memset(trail, '\0', sizeof(trail));
  FILE *fp;
  //initialize the gameboard value for each room
  for (i = 0; i < 7; i++)
  {
    gameboard[i].connectionsCount = 0;
    memset(gameboard[i].roomName, '\0', 100);
  }

  //get directory of most recently touched game folder
  DIR *directory;
  struct stat *dStats;
  struct dirent *lineItem;
  int correct = 1;
  memset(fullpath, '\0', sizeof(fullpath));
  getcwd(fullpath, 100);
  directory = opendir(fullpath);
  if (directory == NULL)
  {
    printf("Error opening directory.");

    exit(1);
  }
  /*continue looping to find most recently added game directory
  Most recent will be saved in folder variable*/
  char *roomsFolder = "rooms";
  //issues with getting file names forced malloc
  dStats = malloc(sizeof(struct stat));
  lineItem = malloc(sizeof(struct dirent));
  while ((lineItem = readdir(directory)))
  {
    //printf("this lineItem string is %s\n", lineItem->d_name);
    if (lineItem->d_name[9] == 'r' && lineItem->d_name[12] == 'm')
    {
      stat(lineItem->d_name, dStats);
      current = dStats->st_mtime;
      if (current > recent)
      {
        recent = current;
        strcpy(folder, lineItem->d_name);
      }
    }
  }
  //move to new directory. Throw error if problem.
  sprintf(fullpath, "%s/%s", fullpath, folder);
  directory = opendir(fullpath);
  memset(fullpath, '\0', sizeof(fullpath));
  getcwd(fullpath, 100);
  if(directory == NULL)
  {
    printf("Error opening up game directory\n");
  }

  /*read through directory and assign room names to any entries larger
  than 3 chars
  */
  index = 0;
  while ((lineItem = readdir(directory)) != NULL)
  {
    //only act on items known be room names
    if (strlen(lineItem->d_name) > 3)
    {
      strcpy(gameboard[index].roomName, lineItem->d_name);
      index++;
    }
  }

  //open room files 1 by and populate our game board.
  for (i = 0; i < 7; i++)
  {
    memset(filesFolder, '\0', sizeof(filesFolder));
    sprintf(filesFolder, "%s/%s", folder, gameboard[i].roomName);
    fp = fopen(filesFolder, "r");
    if(fp == NULL)
    {
      printf("Error opening up file for room %d\n", i);
      exit(1);
    }

    //while loop to continue processing the contents of the file
    while (fgets(line, sizeof(line), fp))
    {
      if (strstr(line, "ROOM NAME") != NULL)
      {
        for (j = 0; j < 10; j++)
        {
          if (strstr(line, rooms[j]) != NULL)
          {
            strcpy(gameboard[i].roomName, rooms[j]);
          }
        }
      }
      //set any connections
      else if (strstr(line, "CONNECTION") != NULL)
      {
        //run through and compare all possible names for connections
        for (j = 0; j < 7; j++)
        {
          if (strstr(line, gameboard[j].roomName) != NULL && gameboard[i].connections[j] == 0)
          {
            gameboard[i].connections[j] = 1;
            gameboard[i].connectionsCount += 1;
            gameboard[j].connections[i] = 1;
            gameboard[j].connectionsCount += 1;
          }
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
    }//end of while loop
  }

  //cleanup
  free(lineItem);
  free(dStats);
  lineItem = NULL;
  dStats = NULL;

  fclose(fp);
  closedir(directory);



  //game board established with rooms created. can begin playing the gameboard
  done = 0;
  int steps = 0;
  int location = begin;
  int period = 0;
  int displayed;
  path[0] = begin;
  sprintf(trail, "%s", gameboard[location].roomName);
  do {
    printf("CURRENT LOCATION: %s\n", gameboard[location].roomName);
    printf("POSSIBLE CONNECTIONS: ");
    period = 0;
    for (j = 0; j < 7; j++)
    {
      displayed = period;
      if (gameboard[location].connections[j] == 1 && j != location)
      {
        printf("%s", gameboard[j].roomName);
        period += 1;
      }

      if (period == gameboard[location].connectionsCount)
        printf(".");
      //only add a comma when a name gets added
      else if (period != displayed)
        printf(", ");
    }
    printf("\nWHERE TO? >");
    getline(&lineEntered, &size, stdin);
    //need to strip away the new line character
    if ((newLine = strchr(lineEntered, '\n')) != NULL)
      *newLine = '\0';
    for (i = 0; i < 7; i++)
    {
      found = 0;
      if (strcmp(lineEntered, gameboard[i].roomName) == 0)
      {
        found = 1;
        if (gameboard[location].connections[i] == 1 && i != location)
        {
          location = i;
          steps += 1;
          path[steps] = i;
          i = 7;
          sprintf(trail, "%s\n%s", trail, gameboard[i].roomName);
        }
        else
        {
          printf("INVALID CONNECTION\n");
        }
      }
    }
    if (strcmp(lineEntered, "time") == 0)
    {
      found = 1;
      current = time(NULL);
      struct tm *tm = localtime(&current);
      printf("%s\n", asctime(tm));
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
    printf("%s\n", gameboard[path[i]].roomName);
  }

  return 0;
}
