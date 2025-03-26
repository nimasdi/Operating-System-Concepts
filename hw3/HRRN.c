#include <stdio.h>

typedef struct
{
    int id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int completed;
} Process;

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

int main()
{

    int sum_burst_time = 0;
    float average_waiting_time = 0;

    int process_count;
    scanf("%d", &process_count);

    Process processes[process_count];

    for (int i = 0; i < process_count; i++)
    {
        scanf("%d %d", &processes[i].arrival_time, &processes[i].burst_time);
    }

    for (int i = 0; i < process_count; i++)
    {
        processes[i].id = i;
        processes[i].completed = 0;
        sum_burst_time += processes[i].burst_time;
    }

    for (int i = 0; i < process_count - 1; i++)
    {
        for (int j = i + 1; j < process_count; j++)
        {
            if (processes[i].arrival_time > processes[j].arrival_time)
            {
                swap(&processes[i], &processes[j]);
            }
        }
    }

    int time = processes[0].arrival_time;
    while (time < sum_burst_time)
    {
        float hrr = -12000;
        int pointer;
        for (int i = 0; i < process_count; i++)
        {
            if (processes[i].arrival_time <= time && processes[i].completed != 1)
            {
                float temp = (processes[i].burst_time + (time - processes[i].arrival_time)) / processes[i].burst_time;
                if (hrr < temp)
                {
                    hrr = temp;
                    pointer = i;
                }
            }
        }

        printf("process %d is executed at %d\n", processes[pointer].id, time);
        time += processes[pointer].burst_time;

        processes[pointer].waiting_time = time - processes[pointer].arrival_time - processes[pointer].burst_time;

        processes[pointer].completed = 1;

        average_waiting_time += processes[pointer].waiting_time;
    }
    printf("average waiting time: %.3f\n", average_waiting_time / process_count);

    return 0;
}

/*
5
3 0
6 2
4 4
5 6
8 2
*/
