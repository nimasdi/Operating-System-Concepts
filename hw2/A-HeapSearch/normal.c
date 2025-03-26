#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

bool findInMinHeap(int *A, int n, int q, int index)
{
    if (n <= 0 || A == NULL)
    {
        printf("Invalid input\n");
        return false; 
    }

    if (index >= n)
        return false;

    if (A[index] > q)
        return false;

    if (A[index] == q)
        return true;

    int left = 2 * index + 1;
    int right = 2 * index + 2;

    return findInMinHeap(A, n, q, left) || findInMinHeap(A, n, q, right);
}

void find(int n, int *A, int q)
{
    if (findInMinHeap(A, n, q, 0))
        printf("Element exists\n");
    else
        printf("Element doesn't exist!\n");
}
