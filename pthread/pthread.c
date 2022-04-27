#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#include "pthread.h"

#define MAX_LINES 5000

char findMinChars(char* line);
  
/* comparison function to use to find the lowest character in each row */
char findMinChars(char* line)
{
    char compChar;

    if(isalpha(line[0]) == 0)
        compChar = 'z';
    else
        compChar = line[0];

    //printf("compChar: %c\n", compChar);
    for(int i = 0; i < strlen(line); i++)
    {
        if(isalpha(line[i]) != 0)
        {
            //printf("line[i]: %c | compChar: %c\n", line[i], compChar);
            if(line[i] < compChar)
            {
                compChar = line[i];
            }
        }
    }
    return compChar;
}

int main()
{
    size_t lineLen = 2500;
    /* don't allow more than 2500 chars in one string */
    char* line = malloc(sizeof(char) *lineLen);
    int lineNum = 0;

    FILE* fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if(!fp)
        return -1;
    
    for(int i = 0; i <= MAX_LINES; i++)
    {
        /* change this to getline() */
        if(getline(&line, &lineLen, fp) == -1)
            break;
        
        //if (fscanf(fp, "%[^\n]\n", line) == EOF)
        //    break;
        printf("Line %d: min char: %c\n", lineNum, findMinChars(line));
        lineNum++;
    }


    return 0;
}
