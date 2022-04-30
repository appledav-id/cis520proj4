#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_LINES 500000

int findMinChars(char *line, int len);
void *thread_func(void *id);

int Num_Threads = 20;
int *min_char;
pthread_mutex_t mutex_min_char;

/* comparison function to use to find the lowest character in each row */
int findMinChars(char *line, int len)
{
    char compChar = '~';
    int i;
    for (i = 0; i < len; i++)
    {
        if ((line[i] > 32) && (line[i] < 127)) //Everything Only between Space and Delete
        {
            if (line[i] < compChar)
            {
                compChar = line[i];
            }
        }
    }
    return (int) compChar;
}

void *thread_func(void *id)
{
    int line_num = 0;
    int c;
    int t_id = *(int*)id;
    int i, j;
    int ret;
    FILE *fp;
    fp = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (!fp)
        exit(-1);
    char *line = malloc(sizeof(char) * 3000);
    if ( line == NULL ) {
        printf("allocation Failure\n");
        return (void*)-1;
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
    min_char[(line_num-1)] = c;

    // run until we are out of lines
    for (i = 0; i < MAX_LINES; i++)
    {
        for (j = 0; j < Num_Threads; j++)
        {
            line_num++;
            ret = getline(&line, &line_len, fp);
            if (ret == -1 || line_num > MAX_LINES)
                break; // this will only get us out of the inner loop
        }
        if (ret == -1 || line_num > MAX_LINES)
            break;
        c = findMinChars(line, ret);
        min_char[(line_num-1)] = c;
    }

    free(line);
    free(id);
    fclose(fp);
    pthread_exit(0);
}

int main()
{
    int i;
    int ret;
    pthread_t threads[Num_Threads];
    min_char = malloc(sizeof(int) * MAX_LINES);
    void *status;

    for (i = 0; i < Num_Threads; i++)
    {
        int *arg = malloc(sizeof(*arg));
        if ( arg == NULL ) {
            printf("allocation Failure\n");
            return -1;
        }
        *arg = i;

        ret = pthread_create(&threads[i], NULL, thread_func, (void *)arg);
        if (ret != 0)
        {
            printf("pthread create error:\n");
            return -1;
        }
    }

    for (i = 0; i < Num_Threads; i++)
    {
        ret = pthread_join(threads[i], &status);
        if (ret != 0)
        {
            printf("pthread join error: %d\n", ret);
            return -1;
        }
    }
    
    for (i = 0; i < MAX_LINES; i++)
    {
        int val = min_char[i];
        printf("%d: %d\n", i+1, val);
    }
    
    free(min_char);
    return 0;
}
