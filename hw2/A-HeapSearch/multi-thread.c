#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

#define NUM_THREADS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int result = -1;

typedef struct
{
    int *array;
    int start;
    int end;
    int target;
    int n;
} SearchArgs;

void *searchMinHeapSection(void *args)
{
    SearchArgs *searchArgs = (SearchArgs *)args;
    int *A = searchArgs->array;
    int start = searchArgs->start;
    int end = searchArgs->end;
    int q = searchArgs->target;
    int n = searchArgs->n;

    for (int i = start; i < end && atomic_load(&result) == -1; i++)
    {
        if (A[i] == q)
        {
            atomic_store(&result, i);
            break;
        }

        if (A[i] > q)
        {
            int leftChild = 2 * i + 1;
            int rightChild = 2 * i + 2;

            while (i < end && (i - start) % 2 == 0)
            {
                i++;
            }
        }
    }

    free(args);
    return NULL;
}

void find_mt(int n, int *A, int q)
{
    atomic_store(&result, -1);

    int sectionSize = (n + NUM_THREADS - 1) / NUM_THREADS;
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int start = i * sectionSize;
        if (start >= n)
            break;

        int end = start + sectionSize;
        if (end > n)
            end = n;

        SearchArgs *args = malloc(sizeof(SearchArgs));
        if (args == NULL)
        {
            printf("Memory allocation failed\n");
            return;
        }

        args->array = A;
        args->start = start;
        args->end = end;
        args->target = q;
        args->n = n;

        if (pthread_create(&threads[i], NULL, searchMinHeapSection, args) != 0)
        {
            printf("Thread creation failed\n");
            free(args);
            return;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int start = i * sectionSize;
        if (start >= n)
            break;
        pthread_join(threads[i], NULL);
    }

    int final_result = atomic_load(&result);
    if (final_result != -1)
    {
        printf("Element exists!\n");
    }
    else
    {
        printf("Element doesn't exist!\n");
    }
}