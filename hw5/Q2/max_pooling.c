#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define M 5  
#define N 5 
#define K 2
#define L 2

#define M_OUT ((M + K - 1) / K)
#define N_OUT ((N + L - 1) / L)

typedef struct
{
    int result[M_OUT][N_OUT];
} SharedData;

const char *shm_name = "/max_pooling_shm";

void print_matrix(int matrix[][N], int rows, int cols, const char *name)
{
    printf("\n%s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%2d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_res_matrix(int matrix[][N_OUT], int rows, int cols, const char *name)
{
    printf("\n%s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%2d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void log_message(const char *format, ...)
{
    time_t now;
    char timestamp[26];
    va_list args;
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    va_start(args, format);
    printf("[%s][PID:%d] ", timestamp, getpid());
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

int max_pool(int matrix[M][N], int startI, int startJ)
{
    log_message("Starting max pooling at position (%d,%d)\n", startI, startJ);
    int max = matrix[startI][startJ];
    log_message("Initial max value: %d\n", max);

    int endI = (startI + K > M) ? M : startI + K;
    int endJ = (startJ + L > N) ? N : startJ + L;

    for (int i = startI; i < endI; i++)
    {
        for (int j = startJ; j < endJ; j++)
        {
            if (matrix[i][j] > max)
            {
                log_message("New max found at (%d,%d): %d -> %d\n", 
                           i, j, max, matrix[i][j]);
                max = matrix[i][j];
            }
        }
    }
    
    log_message("Completed max pooling at position (%d,%d). Final max: %d\n", 
                startI, startJ, max);
    return max;
}

int main()
{
    srand(time(NULL));

    log_message("Starting matrix filling\n");
    int M1[M][N];
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            M1[i][j] = rand() % 9 + 1;
        }
    }

    print_matrix(M1, M, N, "Input Matrix M1");

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));

    SharedData *shared_data = mmap(NULL, sizeof(SharedData),
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < M_OUT; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            log_message("Processing row block %d/%d\n", i + 1, M_OUT);
            for (int j = 0; j < N_OUT; j++)
            {
                shared_data->result[i][j] = max_pool(M1, i * K, j * L);
            }
            exit(0);
        }
    }

    for (int i = 0; i < M_OUT; i++)
    {
        wait(NULL);
    }

    int M2[M_OUT][N_OUT];
    memcpy(M2, shared_data->result, sizeof(M2));

    print_res_matrix(M2, M_OUT, N_OUT, "Output Matrix M2");

    munmap(shared_data, sizeof(SharedData));
    shm_unlink(shm_name);

    return 0;
}