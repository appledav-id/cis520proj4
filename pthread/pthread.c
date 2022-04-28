#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#include "pthread.h"

#define MAX_LINES 50000000000

char findMinChars(char* line);
  
/* comparison function to use to find the lowest character in each row */
char findMinChars(char* line)
{
    char compChar = '~';
    for(int i = 0; i < strlen(line); i++)
    {
        if((line[i] > 32) && (line[i] < 127)) //
        {
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
    char* line = malloc(sizeof(char) *lineLen);
    int lineNum = 0;

    FILE* fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if(!fp)
        return -1;
    
    for(int i = 0; i <= MAX_LINES; i++)
    {
        if(getline(&line, &lineLen, fp) == -1)
            break;

        char c = findMinChars(line);
        printf("Line %d: min char: %c \tcharNum: %d\n", lineNum, c, c);
        lineNum++;
    }

    return 0;
}
