#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

int findMinChars(char *line, int len);
void *thread_func(void *id);

int *Num_Threads;
int *Max_Lines;
int *min_char;

/* comparison function to use to find the lowest character in each row */
int findMinChars(char *line, int len)
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
    return (int)compChar;
}

// This is the function used for threads
void *thread_func(void *id)
{
    int line_num = 0;
    int c;
    int t_id = *(int *)id;
    int i, j;
    int ret;
    FILE *fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (!fp)
        exit(-1);
    char *line = malloc(sizeof(char) * 3000);
    if (line == NULL)
    {
        printf("allocation Failure\n");
        return (void *)-1;
    }
    size_t line_len = sizeof(line);

    // Get to the starting line for the thread
    for (i = 0; i <= t_id; i++)
    {
        line_num++;
        ret = getline(&line, &line_len, fp);
        if (ret == -1)
            break;
    }
    // run function to get first minchar
    c = findMinChars(line, ret);
    min_char[(line_num - 1)] = c;

    // run until we are out of lines
    for (i = 0; i < *Max_Lines; i++)
    {
        for (j = 0; j < *Num_Threads; j++)
        {
            line_num++;
            ret = getline(&line, &line_len, fp);
            if (ret == -1 || line_num > *Max_Lines)
                break; // this will only get us out of the inner loop
        }
        if (ret == -1 || line_num > *Max_Lines)
            break; // breaks out of outside loop
        c = findMinChars(line, ret);
        min_char[(line_num - 1)] = c;
    }

    free(line); 
    free(id); //free the malloc from main args
    fclose(fp);
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 3) //check the input arguments
    {
        printf("%s ThreadCount MaxLineNumber\n", argv[0]);
        return EXIT_FAILURE;
    }

    Num_Threads = malloc(sizeof(int)); 
    if (Num_Threads == NULL)
    {
        printf("allocation Failure\n");
        return -1;
    }

    *Num_Threads = atoi(argv[1]);
    Max_Lines = malloc(sizeof(int));

    if (Max_Lines == NULL)
    {
        printf("allocation Failure\n");
        return -1;
    }

    *Max_Lines = atoi(argv[2]);
    min_char = malloc(sizeof(int) * (*Max_Lines));

    if (min_char == NULL)
    {
        printf("allocation Failure\n");
        return -1;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * (*Num_Threads)); //array to hold thread info
    if (threads == NULL)
    {
        printf("allocation Failure\n");
        return -1;
    }

    clock_t begin = clock(); //Start to time the program

    int i;
    int ret;
    void *status;

    for (i = 0; i < *Num_Threads; i++)
    {
        //needed to allocate this so that can pass in correct id to thread function
        int *arg = malloc(sizeof(*arg)); 
        if (arg == NULL)
        {
            printf("allocation Failure\n");
            return -1;
        }
        *arg = i;

        ret = pthread_create(&threads[i], NULL, thread_func, (void *)arg); //create the thread for funtion passing in arg
        if (ret != 0)
        {
            printf("pthread create error:\n");
            return -1;
        }
    }

    for (i = 0; i < *Num_Threads; i++)
    {
        ret = pthread_join(threads[i], &status); //check to see that threads have closed
        if (ret != 0)
        {
            printf("pthread join error: %d\n", ret);
            return -1;
        }
    }

    for (i = 0; i < *Max_Lines; i++)
    {
        int val = min_char[i]; 
        printf("%d: %d\n", i + 1, val); //print the values for minimum character of each line to the terminal
    }

    clock_t end = clock(); //stop the execution timer
    double run_time = (double)(end - begin); //Calulate the elapsed Time

    printf("\n\n\n{ \"NumThreads\":%d, \"NumLines\":%d, \"RunTime\":%lf }\n", *Num_Threads, *Max_Lines, run_time / CLOCKS_PER_SEC); //Print out values to terminal in JSON Format

    //Free the rest of the memory
    free(min_char);
    free(Max_Lines);
    free(Num_Threads);
    free(threads);
    return 0;
}
