#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include <time.h>

#define MAX_LINE_LENGTH 2000
#define MAX_LINES 1000000

char gFileContents[MAX_LINES][MAX_LINE_LENGTH];

void findMinChars(char* minCharAtLine, int numThreads, int maxLines);

void findMinChars(char* minCharAtLine, int numThreads, int maxLines)
{
    int lineNum = 0;
    char minChar;

    #pragma omp parallel private(lineNum, minChar) num_threads(numThreads)
    {
        lineNum = omp_get_thread_num();

        while(lineNum < maxLines)
        {
            minChar = 127;
            for(int i = 0; i < MAX_LINE_LENGTH; i++)
            {
                if ((gFileContents[lineNum][i] > 32) && (gFileContents[lineNum][i] < 127)) // Everything Only between Space and Delete
                {
                    if (gFileContents[lineNum][i] < minChar)
                    {
                        //printf("%d: %d: Minchar changed: %c | %d => %c | %d\n", omp_get_thread_num(), lineNum, minChar, minChar, gFileContents[lineNum][i], gFileContents[lineNum][i]);
                        minChar = gFileContents[lineNum][i];
                    }
                }
            }

            /* update so other threads can go ahead */
            lineNum += numThreads;

            #pragma omp critical
            {
                minCharAtLine[lineNum - numThreads] = minChar;
            }
        }
        printf("Hello from thread %d\n", omp_get_thread_num());
    }
}


int main(int argc, char** argv)
{
    clock_t start, end, length;
    int numThreads = 4, maxLines = MAX_LINES;
    switch(argc)
    {
        case 2:
            numThreads = atoi(argv[1]);
            maxLines = 1000;
            break;
        case 3:
            numThreads = atoi(argv[1]);
            maxLines = atoi(argv[2]);
            break;
    }
    

    //omp_set_num_threads(numThreads);

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


    /* start timer here */
    start = clock();
    findMinChars(minCharAtLine, numThreads, maxLines);
    end = clock();

    length = (double)(end - start);

    for(int i = 0; i < maxLines; i++)
    {
        printf("Line %d: %c\n", i, minCharAtLine[i]);
    }

    printf("Time taken: %f\n", length);

    return 0;
}
