#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 5000   
#define STRING_SIZE 16
int NUM_THREADS;

char minChars[MAX_LINES];

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

// a wrapper for findMinChars that chunks the data based on the rank 
void handleMinChar(void* rank)
{
    int myID = *((int*) rank);
    int startPos = ((long) myID) * (MAX_LINES / NUM_THREADS);
    int endPos = startPos + (MAX_LINES / NUM_THREADS);

    char* line;

    //TODO: probably use lseek to jump to the correct bytes somehow here

    for(int i = startPos; i < endPos; i++)
    {
        minChars[i] = findMinChars(line);
    }
}


int main(int argc, char* argv[])
{
    size_t lineLen = 2500;
    /* don't allow more than 2500 chars in one string */
    char* line = malloc(sizeof(char) *lineLen);
    int lineNum = 0;

    FILE* fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if(!fp)
        return -1;
    

    int rc, rank, numtasks;

    rc = MPI_Init(&argc, &argv);
    if(rc != MPI_SUCCESS) {
        printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    //update the number of threads for distributing work in other functions
    NUM_THREADS = numtasks;

    //TODO: MPI_Bcast & MPI_Reduce

    if (rank == 0)
    {
        //TODO: print results
    }

    MPI_Finalize();
    return 0;

}