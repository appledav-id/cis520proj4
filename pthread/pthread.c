#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#include "pthread.h"

#define MAX_LINES 1000005

int findMinChars(char *line);
void *thread_func(void *id);

int Num_Threads = 8;
int *min_char;
pthread_mutex_t mutex_min_char;

/* comparison function to use to find the lowest character in each row */
int findMinChars(char *line)
{
    char compChar = '~';
    for (int i = 0; i < strlen(line); i++)
    {
        if ((line[i] > 32) && (line[i] < 127)) //
        {
            if (line[i] < compChar)
            {
                compChar = line[i];
            }
        }
    }
    return compChar;
}

void *thread_func(void *id)
{
    int line_num = 0;
    char c;
    int t_id = *(int*)id;
    int i, j;
    int ret;
    FILE *fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (!fp)
        exit(-1);
    char *line = malloc(sizeof(char) * 2000);
    size_t line_len = sizeof(line);

    // Get to the starting line for the thread
    for (i = 0; i <= t_id-1; i++)
    {
        line_num++;
        if (getline(&line, &line_len, fp) == -1)
            break;
    }
    // run function to get first minchar
    c = findMinChars(line);

    pthread_mutex_lock(&mutex_min_char);
    min_char[line_num-1] = c;
    pthread_mutex_unlock(&mutex_min_char);

    // run until we are out of lines
    for (i = 0; i < MAX_LINES; i++)
    {
        for (j = 0; j < Num_Threads; j++)
        {
            line_num++;
            ret = getline(&line, &line_len, fp);
            if (ret == -1)
                break; // this will only get us out of the inner loop
        }
        if (ret == -1)
            break;
        c = findMinChars(line);
        pthread_mutex_lock(&mutex_min_char);
        min_char[line_num-1] = c;
        pthread_mutex_unlock(&mutex_min_char);
    }

    pthread_exit(NULL);
}

int main()
{
    int i;
    min_char = malloc(sizeof(int) * MAX_LINES);

    pthread_t threads[Num_Threads];
    pthread_attr_t attr;
    void *status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&mutex_min_char, NULL);

    for (i = 0; i <= Num_Threads; i++)
    {
        if (pthread_create(&threads[i], &attr, thread_func, (void *)&i))
        {
            printf("pthread create error:");
            return -1;
        }
    }

    pthread_attr_destroy(&attr);
    for (i = 0; i < Num_Threads; i++)
    {
        if (pthread_join(threads[i], &status))
        {
            printf("pthread join error:");
            return -1;
        }
    }
    pthread_mutex_destroy(&mutex_min_char);

    for (i = 0; i < MAX_LINES; i++)
    {
        int val = min_char[i];
        printf("%d: %d\n", i, val);
    }

    return 0;
}
