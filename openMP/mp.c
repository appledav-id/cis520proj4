#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define MAX_LINE_LENGTH 2000


void findMinChars(int id, char* minCharAtLine, int numThreads, int maxLines);

void findMinChars(int myID, char* minCharAtLine, int numThreads, int maxLines)
{
    printf("Thread id: %d\n", myID);
    char* fileName = "/homes/dan/625/wiki_dump.txt";
    int currentLineNum = 1, iteration = 0;

    char* line;
    size_t numRead;
    char minChar; 

    FILE* fp = fopen(fileName, "r");
    if(!fp)
        return;
    size_t maxLineLength = (size_t)MAX_LINE_LENGTH;
    
    int i;
    #pragma omp private(fp, myID, line, currentLineNum, i, iteration, minChar, numRead, maxLineLength)
    {
        line = malloc(sizeof(char) * MAX_LINE_LENGTH);

        while(currentLineNum < maxLines)
        {    
            /* this will get us to where we need to be */
            for(int i = 0; i < currentLineNum; i++)
                getline(&line, &maxLineLength, fp);
            
            numRead = getline(&line, &maxLineLength, fp);
        
            /* setting the min char to compare against with the highest value in our range */
            minChar = (char)126;
            for(i = 0; i < numRead; i++)
            {
                if ((line[i] > 32) && (line[i] < 127))
                {
                    /* little optimization */
                    if(line[i] == 33)
                    {
                        minChar = 33;
                        break;
                    }
                    if(line[i] < minChar)
                        minChar = line[i];
                }
            }
            
            #pragma omp critical
            {
                currentLineNum = myID + (numThreads * iteration);
                /* should continually update for each iteration */
                iteration++;
                //printf("%d: %c\n", currentLineNum, minChar);
                minCharAtLine[currentLineNum] = minChar;
            }

            //fseek(fp, NUM_THREADS, SEEK_SET);

        } // end while loop

        free(line);
    } // end of #pragma
    
}

int main(int argc, char** argv)
{
    int numThreads = 2, maxLines = 1000;
    /*switch(argc)
    {
        case 2:
            numThreads = atoi(argv[1]);
            maxLines = 1000;
            break;
        case 3:
            numThreads = atoi(argv[1]);
            maxLines = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "%s\n", "Usage: ./mp numThreads maxLines");
            exit(-1);
    }*/
    

    omp_set_num_threads(numThreads);

    char* minCharAtLine = malloc(sizeof(char) * maxLines);
    //memset(minCharAtLine, 0, strlen(minCharAtLine));

    /* start timer here */
    #pragma omp parallel
    {
        findMinChars(omp_get_thread_num(), minCharAtLine, numThreads, maxLines);
    }
    /* stop timer here */

    for(int i = 1; i < maxLines; i++)
    {
        printf("Line %d: %c\n", i, minCharAtLine[i]);
    }

    return 0;
}
