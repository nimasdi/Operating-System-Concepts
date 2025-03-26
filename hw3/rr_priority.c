#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int id;
    int burst_time;
    int priority;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
} Process;

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void sort_by_priority(Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (processes[i].priority < processes[j].priority)
            {
                swap(&processes[i], &processes[j]);
            }
        }
    }
}

void rr_with_priority(Process processes[], int n, int quantum)
{

    int time = 0;
    int completed = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;

    sort_by_priority(processes, n);

    int finished[n];

    for (int i = 0; i < n; i++)
    {
        finished[i] = -1;
        processes[i].waiting_time = 0;
        processes[i].remaining_time = processes[i].burst_time;
    }

    printf("Order of execution : \n");
    while (completed < n)
    {

        int next_i;

        for (int i = 0; i < n; i++)
        {
            if (finished[i] < 0)
            {
                next_i = i;
                break;
            }
        }

        if (processes[next_i].remaining_time > 0)
        {
            int execution_time = (processes[next_i].remaining_time > quantum) ? quantum : processes[next_i].remaining_time;

            processes[next_i].remaining_time -= execution_time;
            time += execution_time;

            printf("P%d ", processes[next_i].id);

            if (processes[next_i].remaining_time == 0)
            {
                processes[next_i].turnaround_time = time;
                processes[next_i].waiting_time = processes[next_i].turnaround_time - processes[next_i].burst_time;
                total_waiting_time += processes[next_i].waiting_time;
                total_turnaround_time += processes[next_i].turnaround_time;
                finished[next_i]++;
                completed++;
            }
        }
    }

    printf("\nProcess\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst_time,
               processes[i].waiting_time, processes[i].turnaround_time);
    }

    printf("\nAverage Waiting Time : %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time : %.2f\n", total_turnaround_time / n);
}

int main()
{
    int n, quantum;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    
    Process processes[n];

    for (int i = 0; i < n; i++)
    {
        printf("Enter burst time for process P%d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
        printf("Enter priority for process P%d: ", i + 1);
        scanf("%d", &processes[i].priority);
        processes[i].id = i + 1;
    }
    printf("Enter the time quantum: ");
    scanf("%d", &quantum);

    rr_with_priority(processes, n, quantum);

    return 0;
}

