#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define MAX_LINES 5000



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
#if 0
    /* at this point, we have found the minimum char */
    data.val = compChar;
    data.count = 0;
    
    for(int i = 0; i < strlen(line); i++)
    {
        if(line[i] == compChar)
            data.count++;
    }
    return data;
#endif 
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
    
    for(int i = 0; i < MAX_LINES; i++)
    {
        /* change this to getline() */
        if(getline(&line, &lineLen, fp) == -1)
            break;
        
        //if (fscanf(fp, "%[^\n]\n", line) == EOF)
        //    break;
        printf("%d: min char: %c\n", lineNum, findMinChars(line));
        lineNum++;
    }


    return 0;
}
