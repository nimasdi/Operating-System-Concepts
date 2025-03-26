#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

int check_prime(int num)
{
    if (num <= 1)
        return 0;
    if (num == 2)
        return 1;
    if (num % 2 == 0)
        return 0;

    for (int i = 3; i * i <= num; i += 2)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

void find_primes(int start, int end, int pipe_write)
{
    for (int i = start; i < end; i++)
    {
        if (check_prime(i))
        {
            write(pipe_write, &i, sizeof(i));
        }
    }
}

long ANSWER_UP_TO = 100000000;
uint64_t total_sum = 0;
int pipe_fd[2];

int main()
{

    long nproc = sysconf(_SC_NPROCESSORS_ONLN);
    long child_range = ANSWER_UP_TO / nproc;

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    for (long i = 0; i < nproc; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            return 1;
        }

        if (pid == 0)
        {
            close(pipe_fd[0]);
            int start = i * child_range;
            int end = (i == nproc - 1) ? ANSWER_UP_TO : (i + 1) * child_range;
            find_primes(start, end, pipe_fd[1]);
            close(pipe_fd[1]);
            exit(0);
        }
    }

    close(pipe_fd[1]);

    int prime;
    while (read(pipe_fd[0], &prime, sizeof(prime)) > 0)
    {
        total_sum += prime;
    }
    close(pipe_fd[0]);

    while (wait(NULL) > 0)
        ;

    printf("sum is: %lu\n", total_sum);

    return 0;
}
