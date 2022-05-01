#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include <time.h>

#define MAX_LINE_LENGTH 2000


void findMinChars(char* minCharAtLine, int numThreads, int maxLines);

void findMinChars(char* minCharAtLine, int numThreads, int maxLines)
{
    char* fileName = "/homes/dan/625/wiki_dump.txt";
    int currentLineNum = 0, iteration = 0;

    char* line;
    int numRead;
    char minChar; 

    size_t maxLineLength = (size_t)MAX_LINE_LENGTH;
    
    int i;
    #pragma omp parallel private( line, currentLineNum, i, iteration, minChar, numRead, maxLineLength) num_threads(numThreads)
    {
        FILE* fp = fopen(fileName, "r");

        int myID = omp_get_thread_num();
        printf("Myid: %d\n", myID);
        currentLineNum = myID;
        line = malloc(sizeof(char) * MAX_LINE_LENGTH);

        while(currentLineNum < maxLines)
        {    
            /* this will get us to where we need to be */
            for(int i = 0; i < numThreads - 1; i++)
            {
                numRead = getline(&line, &maxLineLength, fp);
                if(numRead == -1)
                    break;
            }
            
            numRead = getline(&line, &maxLineLength, fp);
            if(numRead == -1)
            {
                minChar = 126;
                continue;
            }
        
            /* setting the min char to compare against with the highest value in our range */
            minChar = (char)126;
            for(i = 0; i < numRead; i++)
            {
                if ((line[i] > 32) && (line[i] < 127))
                {
                    //printf("ID %d line[i]: %c\n", myID, line[i]);
                    /* little optimization */
                    if(line[i] == 33)
                    {
                        minChar = 33;
                        //printf("ID %d: %d | Found ! - Min char changed: %c\n", myID, currentLineNum + (numThreads * iteration),  minChar);
                        break;
                    }
                    if(line[i] < minChar)
                    {
                        minChar = line[i];
                        //printf("ID %d: %d | Min char changed: %c\n", myID, currentLineNum + (numThreads * iteration), minChar);
                    }
                }
                //printf("ID %d: %d | Min char right now: %c | %d\n", myID, currentLineNum + (numThreads * iteration), minChar, (int)minChar);
            }
            
            #pragma omp critical
            {
                currentLineNum = myID + (numThreads * iteration);
                /* should continually update for each iteration */
                iteration++;
                printf("ID %d: %d | %c\n", myID, currentLineNum, minChar);
                minCharAtLine[currentLineNum] = minChar;
            }


        } // end while loop

        fclose(fp);
        free(line);
    } // end of #pragma

}

int main(int argc, char** argv)
{
    clock_t start, end, length;
    int numThreads = 2, maxLines = 1000;
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
    

    omp_set_num_threads(numThreads);

    char* minCharAtLine = malloc(sizeof(char) * maxLines);
    //memset(minCharAtLine, 0, strlen(minCharAtLine));

    /* start timer here */
    start = clock();
    findMinChars(minCharAtLine, numThreads, maxLines);
    end = clock();

    length = (double)(end - start) / CLOCKS_PER_SEC ;

    for(int i = 0; i < maxLines; i++)
    {
        printf("Line %d: %c\n", i, minCharAtLine[i]);
    }

    printf("Time taken: %5f\n", length);

    return 0;
}
