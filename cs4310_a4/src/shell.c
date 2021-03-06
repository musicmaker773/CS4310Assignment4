//Code for the the shell
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/dir.h>
#include<sys/param.h>
#include<readline/readline.h>
#include<readline/history.h>

#include <fcntl.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

#define clear() printf("\033[H\033[J")

// Greeting shell during startup
void init_shell()
{
    
    printf("\n\n\n\n***********************"
           "************************");
    printf("\n\n\n\t****TEAM PENULTIMATE SHELL****");
    printf("\n\n  -BY BRYCE QUINTANA, PAUL CHEW, & ERIN CHON-");
    printf("\n\n\n\n*******************"
           "****************************");
    char* username = getenv("USER");
    printf("\n\n\nUSER is: @%s", username);
    printf("\n");
    sleep(1);
    
}

// Function to take input
int takeInput(char* str)
{
    char* buf;
    
    buf = readline(" >>> ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

// Function to print Current Directory.
void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\n%s", cwd);
}

// Function where the system command is executed
void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork();
    
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;
    
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }
    
    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();
        
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }
        
        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Help command builtin
void openHelp()
{
    puts("\n***WELCOME TO TEAM PENULTIMATE SHELL HELP***"
         "\nList of Commands supported:"
         "\n>cd"
         "\n>pwd"
         "\n>mkdir"
         "\n>rmdir"
         "\n>ls"
         "\n>cp"
         "\n>hello"
         "\n>exit"
         "\n>color"
         "\n>pipe handling");
    
    return;
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    struct dirent **files;
    char cwd[1024];
    int count, j;
    
    int NoOfOwnCmds = 11, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;
    
    FILE *fptr1, *fptr2;
    char ch;
    
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "mkdir";
    ListOfOwnCmds[5] = "rmdir";
    ListOfOwnCmds[6] = "pwd";
    ListOfOwnCmds[7] = "ls";
    ListOfOwnCmds[8] = "cp";
    ListOfOwnCmds[9] = "color";
    ListOfOwnCmds[10] = "clear";
    
    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
    
    switch (switchOwnArg) {
        case 1:
            printf("\nGoodbye\n");
            exit(0);
        case 2:
            if(chdir(parsed[1]) < 0) {
                printf("\nCould not change directory..");
            }
            return 1;
        case 3:
            openHelp();
            return 1;
        case 4:
            username = getenv("USER");
            printf("\nHello %s."
                   "\nUse the help command to know more..\n",
                   username);
            return 1;
        case 5:
            if(mkdir(parsed[1], 0777) < 0) {
                printf("\nCould not make new directory..");
            }
            return 1;
        case 6:
            if(rmdir(parsed[1]) < 0) {
                printf("\nCould not remove directory..");
            }
            return 1;
        case 7:
            printDir();
            printf("\n");
            return 1;
        case 8:
            if (!getcwd(cwd, sizeof(cwd))) {
                printf("\nCould not get pathname...");
                return 1;
            }
            
            count = scandir(cwd, &files, NULL, alphasort);
            if(count <= 3) {
                printf("No files in this directory\n");
                return 1;
            }
            for (j = 1; j < count + 1; ++j)
                if ((strcmp(files[j-1]->d_name, ".") != 0) && (strcmp(files[j-1]->d_name, "..") != 0) && (strcmp(files[j-1]->d_name, ".DS_Store") != 0) ) {
                    printf("%s  ",files[j-1]->d_name);
                }
            printf("\n");
            
            return 1;
        case 9:
            fptr1 = fopen(parsed[1], "r");
            if (fptr1 == NULL)
            {
                printf("Cannot open file %s \n", parsed[1]);
                return 1;
            }
            
            fptr2 = fopen(parsed[2], "w");
            if (fptr2 == NULL)
            {
                printf("Cannot open file %s \n", parsed[2]);
                return 1;
            }
            
            ch = fgetc(fptr1);
            while (ch != EOF)
            {
                fputc(ch, fptr2);
                ch = fgetc(fptr1);
            }
            
            printf("\nContents copied to %s", parsed[2]);
            
            fclose(fptr1);
            fclose(fptr2);
            
            return 1;
        case 10:
            
            if(strcmp(parsed[1], "red") == 0) {
                printf("\033[0;31m");
            }
            else if(strcmp(parsed[1], "green") == 0) {
                printf("\033[0;32m");
            }
            else if(strcmp(parsed[1], "blue") == 0) {
                printf("\033[0;34m");
            }
            else if(strcmp(parsed[1], "yellow") == 0) {
                printf("\033[0;33m");
            }
            else if(strcmp(parsed[1], "reset") == 0) {
                printf("\033[0m");
            }
            else {
                printf("\nColor not identified.");
                return 1;
            }
            printf("\nNow, the color is %s", parsed[1]);
            return 1;
        case 11:
            clear();
            return 1;
        default:
            break;
    }
    
    return 0;
}

// function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
    
    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}

// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;
    
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
        
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char* str, char** parsed, char** parsedpipe)
{
    
    char* strpiped[2];
    int piped = 0;
    
    piped = parsePipe(str, strpiped);
    
    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
        
    } else {
        
        parseSpace(str, parsed);
    }
    
    if (ownCmdHandler(parsed))
        return 0;
    else
        return 1 + piped;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    init_shell();
    
    while (1) {
        // print shell line
        printDir();
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString, parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.
        
        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);
        
        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
