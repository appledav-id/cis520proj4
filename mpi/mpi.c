/* kstat --me for the beocat queue
   sbatch sbatch_script.sh

ntasks -> threads, nodes -> basically a cpu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_LINES 100   
int NUM_THREADS;
size_t lineLen = 2500;

char minChars[MAX_LINES];

/* comparison function to use to find the lowest character in each row */
char findMinChars(char* line, int len)
{
    char compChar = '~';
    int i;
    for (i = 0; i < len; i++)
    {
        if ((line[i] > 32) && (line[i] < 127)) // Everything Only between Space and Delete
        {
            if (line[i] < compChar)
            {
                compChar = line[i];
                if (compChar == 33)
                {
                    break;
                }
            }
        }
    }
    return compChar;
}

// a wrapper function that handles the calculations for the threads 
int handleMinChar(int offset, int chunk, int myID)
{

    /* don't allow more than 2500 chars in one string */
    char* line = (char*)malloc(sizeof(char) *lineLen);
    int length;
    FILE* fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if(!fp)
        return 0;

    // move through the file to get to the starting postition
    for(int i = 0; i < offset; i++)
    {
        // stop if it accidently hits the end of the file
        if(getline(&line, &lineLen, fp) == -1)
            break;
    }

    // do the calculation for the lines that are this thread's responsibility
    for(int i = offset; i < offset + chunk; i++)
    {
        length = getline(&line, &lineLen, fp);
        if(length == -1)
            break;
        minChars[i] = findMinChars(line, length);
    }
    
    free(line);
    fclose(fp);
    return 1;
}


// general structure of this code was inspired by: https://hpc-tutorials.llnl.gov/mpi/examples/mpi_array.c
int main(int argc, char* argv[])
{

    int rc, rank, numtasks;
    int ROOT = 0;   //this is the root rank
    int chunksize, offset, leftover, tag1, tag2, destThread, srcThread;
    MPI_Status status;

    rc = MPI_Init(&argc, &argv);
    if(rc != MPI_SUCCESS) {
        printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    //update varialbes now that threads are initialized
    NUM_THREADS = numtasks;
    chunksize = MAX_LINES / NUM_THREADS;
    leftover = MAX_LINES % NUM_THREADS;
    tag1 = 2;
    tag2 = 1;


    /*************************** Root thread tasks *********************************************/
    if(rank == ROOT)
    {
        //Root thread handles its own chunk and the leftovers
        offset = chunksize + leftover;
        for (destThread = 1; destThread < NUM_THREADS; destThread++)
        {   
            //send the part of the array and the offset values to the other threads
            MPI_Send(&offset, 1, MPI_INT, destThread, tag1, MPI_COMM_WORLD);
            MPI_Send(&minChars[offset], chunksize, MPI_BYTE, destThread, tag2, MPI_COMM_WORLD);
            offset = offset + chunksize;
        }

        //master does it's part of the work
        offset = 0;
        if(!handleMinChar(offset, chunksize + leftover, rank))
        {
            printf("rank %d failed\n", rank);
        }

        //wait to recieve results from the other threads
        for(int i = 1; i < NUM_THREADS; i++)
        {
            srcThread = i;
            MPI_Recv(&offset, 1, MPI_INT, srcThread, tag1, MPI_COMM_WORLD, &status);
            MPI_Recv(&minChars[offset], chunksize, MPI_BYTE, srcThread, tag2, MPI_COMM_WORLD, &status);
        }

        //MPI_Reduce??????????dakj:JAFOAHAJC:NIULAHD;ajflusdv;lowAFLuibaw;oifsdluidvluboi;wa;odocs

        //print results at the end
        for(int i = 0; i < MAX_LINES; i ++)
        {
            printf("Line %d: min char: %c \tcharNum: %d\n", i, minChars[i], minChars[i]);
        }
    }

    /*************************** Non-Root thread tasks *********************************************/
    if(rank > ROOT)
    {
        //recieve this thread's portion of the work
        srcThread = ROOT;
        MPI_Recv(&offset, 1, MPI_INT, srcThread, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&minChars[offset], chunksize, MPI_BYTE, srcThread, tag2, MPI_COMM_WORLD, &status);

        //do the work
        if(!handleMinChar(offset, chunksize + leftover, rank))
        {
            printf("rank %d failed\n", rank);
        }

        //send the results back to master
        destThread = ROOT;
        MPI_Send(&offset, 1, MPI_INT, destThread, tag1, MPI_COMM_WORLD);
        MPI_Send(&minChars[offset], chunksize, MPI_BYTE, destThread, tag2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;

}