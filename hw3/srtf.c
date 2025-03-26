#include <stdio.h>
#include <limits.h>

typedef struct
{
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completed;
} Process;

int main()
{

    int process_count, i;
    float avg_waiting_time, avg_turnaround_time;

    printf("Enter the number of processes : ");
    scanf("%d", &process_count);

    Process processes[process_count];

    for (i = 0; i < process_count; i++)
    {
        processes[i].id = i + 1;

        printf("Enter arrival time for process P%d : ", processes[i].id);
        scanf("%d", &processes[i].arrival_time);

        printf("Enter burst time for process P%d : ", processes[i].id);
        scanf("%d", &processes[i].burst_time);

        processes[i].remaining_time = processes[i].burst_time;
        processes[i].completed = 0;
    }

    int current_time, completed_processes;
    current_time = 0;
    completed_processes = 0;
    while (completed_processes < process_count)
    {
        int min_idx = -1, min_remaining_time = INT_MAX;
        for (i = 0; i < process_count; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].completed && processes[i].remaining_time < min_remaining_time)
            {
                min_idx = i;
                min_remaining_time = processes[i].remaining_time;
            }
        }

        if (min_idx == -1)
        {
            current_time++;
            continue;
        }

        processes[min_idx].remaining_time--;
        current_time++;

        if (processes[min_idx].remaining_time == 0)
        {
            processes[min_idx].completed = 1;
            processes[min_idx].turnaround_time = current_time - processes[min_idx].arrival_time;
            processes[min_idx].waiting_time = processes[min_idx].turnaround_time - processes[min_idx].burst_time;
            completed_processes++;
        }
    }

    avg_waiting_time = 0;
    avg_turnaround_time = 0;
    for (i = 0; i < process_count; i++)
    {
        avg_waiting_time += processes[i].waiting_time;
        avg_turnaround_time += processes[i].turnaround_time;
    }
    avg_waiting_time /= process_count;
    avg_turnaround_time /= process_count;


    printf("Process\t\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (i = 0; i < process_count; i++)
    {
        printf("P%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time);
    }
    printf("Average waiting time: %.2f\n", avg_waiting_time);
    printf("Average turnaround time: %.2f\n", avg_turnaround_time);

    return 0;
}
