#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stdio.h>

#define NUM_PROCESSES 4

void search(int *A, int start, int end, int q, int *result)
{
    for (int i = start; i < end; i++)
    {
        if (A[i] == q)
        {
            if (*result == -1 || i < *result)
            {
                *result = i;
            }
            exit(0);
        }
    }
    exit(0);
}

void find_mp(int n, int *A, int q)
{
    int sectionSize = n / NUM_PROCESSES;

    int *result = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *result = -1;

    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        int start = i * sectionSize;
        int end = (i == NUM_PROCESSES - 1) ? n : start + sectionSize;

        pid_t pid = fork();
        if (pid == 0)
        {
            search(A, start, end, q, result);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        wait(NULL);
    }

    if (*result != -1)
    {
        printf("Element exists!\n");
    }
    else
    {
        printf("Element doesn't exist!\n");
    }

    munmap(result, sizeof(int));
}
