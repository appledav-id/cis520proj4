#include <stdio.h>
#include <omp.h>

int anotherFunc()
{
    int i = 0;
    return i;
}

int main()
{
    #pragma omp parallel
    {
        printf("Hello World!\n");
    }
    return 0;
}
