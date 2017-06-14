#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void catchSIGINT(int signo)
{
  char* message = "Caught SIGINT, sleeping for 5 seconds\n";
  write(STDOUT_FILENO, message, 38);
  raise(SIGUSR2);
  sleep(5);
}

void catchSIGTSTP(int signo)
{
  char* message = "Caught SIGTSTP, exiting!\n";
  write(STDOUT_FILENO, message, 25);
  exit(0);
}


int main()
{
  pid_t spawnPid = -5;
  int childExitMethod = -5;
  //when quit is set to 0 in "exit" scenario program ends
  int quit = 1;
  int i;
  int numCharsEntered = -5;
  int reroute = 0;
  int result, targetFD, sourceFD;
  char buffer[20];
  char* word;
  char space[] = " ";
  size_t bufferSize;
  char *lineEntered;


  struct sigaction SIGINT_action = {{0}}, SIGTSTP_action = {{0}}, ignore_action = {{0}};

  SIGINT_action.sa_handler = catchSIGINT;
  sigfillset(&SIGINT_action.sa_mask);
  SIGINT_action.sa_flags = SA_RESTART;//was 0

  SIGTSTP_action.sa_handler = catchSIGTSTP;
  sigfillset(&SIGTSTP_action.sa_mask);
  SIGTSTP_action.sa_flags = SA_RESTART;//was 0

  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);
  //sigaction(SIGTERM, &ignore_action, NULL);
  //sigaction(SIGHUP, &ignore_action, NULL);
  //sigaction(SIGQUIT, &ignore_action, NULL);

  fflush(stdin);
  do {
    //initialize variables for user input
    bufferSize = 0;
    lineEntered = NULL;
    //cleanup before and after capture
    fflush(stdout);
    printf(": ");
    numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
    lineEntered[numCharsEntered - 1] = '\0';
    fflush(stdin);
    //break up input into its components
    word = strtok(lineEntered, space);
    //printf("We have the word as %s\n", word);
    //printf("strcmp returns this %d\n", strcmp(word, "exit"));
    if (strcmp(word, "exit") == 0)
    {
      exit(0);
    }
    spawnPid = fork();
    switch (spawnPid)
    {
      //if forking failed
      case -1:
      {
        perror("Child execution fail\n");
        exit(1);
        break;
      }
      //child process to execute ls command
      case 0:
      {
        if (strcmp(word, "ls") == 0)
        {
          word = strtok(NULL, space);
          printf("The 2nd word is %s\n", word);
          if (word != NULL)
            reroute = 1;
          if (reroute == 1)
          {
            word = strtok(NULL, space);
            printf("The file we are writing to is %s\n", word);
            targetFD = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            result = dup2 (targetFD, 1);
            if (result == -1) { perror("source dup2()"); exit(2); }
            if (execlp("ls", "ls", NULL) == -1)
            {
              printf("Error executing ls command.\n");
              exit(1);
            }
          }
          //baseline condition of  simple ls
          else
          if (execlp("ls", "ls", NULL) == -1)
          {
            printf("Error executing ls command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "pwd") == 0)
        {
          if (execlp("pwd", "pwd", NULL) == -1)
          {
            printf("Error executing pwd command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "status") == 0)
        {
          if (execlp("status", "status", "&", NULL) == -1)
          {
            printf("Error executing status command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "cat") == 0)
        {
          word = strtok(NULL, space);
          if (execlp("cat", "cat", word, NULL) == -1)
          {
            printf("Error executing cat command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "mkdir") == 0)
        {
          word = strtok(NULL, space);
          if (execlp("mkdir", "mkdir", word, NULL) == -1)
          {
            printf("Error executing mkdir command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "cd") == 0)
        {
          word = strtok(NULL, space);
          printf("2nd word for cd is %s\n", word);
          if (strcmp(word, NULL) == 0)
          {
            if (execlp("cd", "cd", NULL) == -1)
            {
              printf("Error executing cd command.\n");
              exit(1);
            }
          }
          else
          {
            if (execlp("cd", "cd", word, NULL) == -1)
            {
              printf("Error executing cd command.\n");
              exit(1);
            }
          }
        }
        else if (strcmp(word, "sleep") == 0)
        {
          word = strtok(NULL, space);
          memset(buffer, '\0', 100);
          strcpy(buffer, word);
          word = strtok(NULL, space);
          if (strcmp(word, NULL) == 0)
          {
            if (execlp("sleep", "sleep", buffer, NULL) == -1)
            {
              printf("Error executing sleep command.\n");
              exit(1);
            }
          }
          else
          {
            if (execlp("sleep", "sleep", buffer, word, NULL) == -1)
            {
              printf("Error executing sleep command.\n");
              exit(1);
            }
          }
        }
        else if (strcmp(word, "pkill") == 0)
        {
          word = strtok(NULL, space);
          if (execlp("pkill", "pkill", word, NULL) == -1)
          {
            printf("Error executing pkill command.\n");
            exit(1);
          }
        }
        else if (strcmp(word, "wc") == 0)
        {
          //nest in as necessary for number of arguments
          word = strtok(NULL, space);
          if (strcmp(word, "<") == 0)
          {
            word = strtok(NULL, space);
            if (strcmp(word, "<") == 0)
            {
              memset(buffer, '\0', 100);
              word = strtok(NULL, space);
              strcpy(buffer, word);
              // now check if there's redirect out
              word = strtok(NULL, space);
              if (strcmp(word, NULL) != 0)
              {
                //move beyond redirect symbol to capture output name
                word = strtok(NULL, space);
                targetFD = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                result = dup2 (targetFD, 1);
                if (result == -1) { perror("target dup2()"); exit(2); }
                sourceFD = open(buffer, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                result = dup2 (sourceFD, 0);
                if (result == -1) { perror("source dup2()"); exit(2); }
                if (execlp("wc", "wc", NULL) == -1)
                {
                  printf("Error executing pkill command.\n");
                  exit(1);
                }
              }
              else
              {
                sourceFD = open(buffer, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                result = dup2 (targetFD, 1);
                if (result == -1) { perror("source dup2()"); exit(2); }
                if (execlp("wc", "wc", buffer, NULL) == -1)
                {
                  printf("Error executing pkill command.\n");
                  exit(1);
                }
              }
            }
          }
        }//end of else if for wc
        if (lineEntered[0] == '#')
        {
          printf("%s\n",lineEntered);
        }
        else
        {
          printf("%s: no such file or directory\n", lineEntered);
          exit(1);
        }
        exit(0);
      }
      //have parent wait for child to terminate
      default:
      {
        waitpid(spawnPid, &childExitMethod, 0);
      }
    }//end of switch


    free(lineEntered);
    lineEntered = NULL;
  } while(quit != 0);

  //cleanup and exit program
  return 0;
}
