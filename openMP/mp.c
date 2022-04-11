#include <stdio.h>
#include <omp.h>

int anotherFunc()
{
    printf("Hello World\n");
    return 0;
}

int main()
{
    anotherFunc();
    return 0;
}
