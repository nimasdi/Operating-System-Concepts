#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    int id;
    int burst_time;
    int arrival_time;
    int waiting_time;
    int turnaround_time;
} Process;

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void sjf(Process *processes, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < n; j++)
        {
            if (processes[j].burst_time < processes[min_idx].burst_time ||
                (processes[j].burst_time == processes[min_idx].burst_time && processes[j].arrival_time < processes[min_idx].arrival_time))
            {
                min_idx = j;
            }
        }
        swap(&processes[min_idx], &processes[i]);
    }
}

int main()
{
    int process_count;
    printf("Enter the number of processes: ");
    scanf("%d", &process_count);

    Process processes[process_count];
    for (int i = 0; i < process_count; i++)
    {
        processes[i].id = i + 1;
        printf("Enter burst time for P%d: ", processes[i].id);
        scanf("%d", &processes[i].burst_time);
        processes[i].arrival_time = 0;
    }

    sjf(processes, process_count);

    int current_time = 0;
    for (int i = 0; i < process_count; i++)
    {
        if (current_time < processes[i].arrival_time)
        {
            current_time = processes[i].arrival_time;
        }
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        current_time += processes[i].burst_time;
        processes[i].turnaround_time = current_time - processes[i].arrival_time;
    }

    printf("Process\t\tBurst Time\tWaiting Time\tTurnaround Time\n");
    int total_waiting_time = 0, total_turnaround_time = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("P%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time);
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / process_count);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / process_count);

    return 0;
}
