#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>


#define N (1 << 20) // 2^20
#define SHM_SIZE (N * sizeof(int))

void merge(int arr[], int l, int m, int r)
{
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    memcpy(L, &arr[l], n1 * sizeof(int));
    memcpy(R, &arr[m + 1], n2 * sizeof(int));

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
    {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void sortSection(int *arr, int start, int end)
{
    mergeSort(arr, start, end);
    exit(0);
}

void mergeSection(int *arr, int left, int mid, int right)
{
    merge(arr, left, mid, right);
    exit(0);
}

void parMergeSort(int *arr, int n, int M)
{
    int chunk_size = n / M;
    pid_t *pids = (pid_t *)malloc(M * sizeof(pid_t));

    // sorting
    for (int i = 0; i < M; i++)
    {
        int start = i * chunk_size;
        int end = (i == M - 1) ? n - 1 : (i + 1) * chunk_size - 1;

        pids[i] = fork();
        if (pids[i] == 0)
        {
            sortSection(arr, start, end);
        }
    }

    // waiting for childs
    for (int i = 0; i < M; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    free(pids);

    // merging
    for (int size = chunk_size; size < n; size *= 2)
    {
        int num_merges = (n + 2 * size - 1) / (2 * size);
        pid_t *merge_pids = (pid_t *)malloc(num_merges * sizeof(pid_t));

        for (int i = 0; i < num_merges; i++)
        {
            int left = i * 2 * size;
            int mid = left + size - 1;
            int right = (left + 2 * size - 1 < n - 1) ? left + 2 * size - 1 : n - 1;

            if (mid < right)
            {
                merge_pids[i] = fork();
                if (merge_pids[i] == 0)
                {
                    mergeSection(arr, left, mid, right);
                }
            }
        }

        // waiting for childs
        for (int i = 0; i < num_merges; i++)
        {
            if (i * 2 * size + size - 1 < n - 1)
            {
                waitpid(merge_pids[i], NULL, 0);
            }
        }
        free(merge_pids);
    }
}

double getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main()
{
    key_t key = IPC_PRIVATE;
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget failed");
        exit(1);
    }

    int *arr = (int *)shmat(shmid, NULL, 0);
    if (arr == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    srand(time(NULL));
    for (int i = 0; i < N; i++)
    {
        arr[i] = rand() % 100 + 1;
    }

    int *arr_copy = (int *)malloc(N * sizeof(int));
    memcpy(arr_copy, arr, N * sizeof(int));

    printf("Finding the best M for N = %d:\n", N);
    printf("| Number of Processes (M) | Time (seconds) |\n");
    printf("|------------------------|----------------|\n");

    double best_time = 1e9;
    int best_m = 1;

    for (int M = 1; M <= 32768; M *= 2)
    {
        memcpy(arr, arr_copy, N * sizeof(int)); // reset array for each test

        double start = getTime();
        parMergeSort(arr, N, M);
        double end = getTime();

        double time_taken = end - start;
        printf("| %-20d | %-14.3f |\n", M, time_taken);

        if (time_taken < best_time)
        {
            best_time = time_taken;
            best_m = M;
        }
    }

    printf("\nBest M: %d with time: %.3f seconds\n", best_m, best_time);

    shmdt(arr);
    shmctl(shmid, IPC_RMID, NULL);
    free(arr_copy);

    return 0;
}