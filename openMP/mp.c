#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define MAX_LINE_LENGTH 2000
#define MAX_LINES 1000000

char gFileContents[MAX_LINES][MAX_LINE_LENGTH];

static void findMinChars(char* minCharAtLine, int numThreads, int maxLines)
{
    int lineNum = 0;
    char minChar;

    #pragma omp parallel private(lineNum, minChar) num_threads(numThreads)
    {
        lineNum = omp_get_thread_num();

        while(lineNum < maxLines)
        {
            /* set to max value each new line */
            minChar = 127;
            for(int i = 0; i < MAX_LINE_LENGTH; i++)
            {
                /* only look in valid range */
                if ((gFileContents[lineNum][i] > 32) && (gFileContents[lineNum][i] < 127)) // Everything Only between Space and Delete
                {
                    if (gFileContents[lineNum][i] < minChar)
                    {
                        minChar = gFileContents[lineNum][i];
                    }
                }
            }

            /* update so other threads can go ahead */
            lineNum += numThreads;

            /* wait for each thread to write to file */
            #pragma omp critical
            {
                minCharAtLine[lineNum - numThreads] = minChar;
            }
        }
    }
}


int main(int argc, char** argv)
{
    /* adjust numThreads as needed. Originally, this was supposed to be
         determined via command line args, but this is easier for consistency */
    int numThreads = 20, maxLines = MAX_LINES;
    

    char* minCharAtLine = malloc(sizeof(char) * maxLines);
    char* lineRead = malloc(sizeof(char) * MAX_LINE_LENGTH);
    
    int numRead;
    
    /* need this for getline */
    size_t maxLineLength = MAX_LINE_LENGTH;

    FILE* fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if(!fp)
        return -1;
    
    for(int i = 0; i < maxLines; i++)
    {
        numRead = getline(&lineRead, &maxLineLength, fp);
        if(numRead == -1)
            break;
        /* i know this is gross but its all I could do */
        strncpy(gFileContents[i], lineRead, MAX_LINE_LENGTH); 
    }

    
    /* do the threading and calculation stuff */
    findMinChars(minCharAtLine, numThreads, maxLines);

    for(int i = 0; i < maxLines; i++)
    {
        /* if you want to see output, uncomment this out */
        printf("Line %d: min char: %c\tcharNum: %d\n", i, minCharAtLine[i], minCharAtLine[i]);
    }


    return 0;
}
