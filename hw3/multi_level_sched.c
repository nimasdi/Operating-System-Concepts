#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define q1_quantum 8
#define q2_quantum 16

typedef struct Process
{
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int rem_after_interrupt;
    bool in_q1;
    bool in_q2;
    bool in_q3;
    bool completed;
} Process;

void enqueue(Process **queue, Process *p, int *rear)
{
    queue[(*rear)++] = p;
}

Process *dequeue(Process **queue, int *front, int *rear)
{
    if (*front == *rear)
    {
        return NULL;
    }
    return queue[(*front)++];
}

Process *get_top(Process **queue, int *front, int *rear)
{
    if (*front == *rear)
    {
        return NULL;
    }
    return queue[(*front)];
}

void swap(Process *a, Process *b)
{
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void sort(Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (processes[j].arrival_time < processes[i].arrival_time)
            {
                swap(&processes[i], &processes[j]);
            }
        }
    }
}

void mlfq(Process processes[], int n)
{

    Process **queue1 = (Process **)malloc(n * sizeof(Process *));
    Process **queue2 = (Process **)malloc(n * sizeof(Process *));
    Process **queue3 = (Process **)malloc(n * sizeof(Process *));
    int front1 = 0, rear1 = 0;
    int front2 = 0, rear2 = 0;
    int front3 = 0, rear3 = 0;

    int time = 0;
    int completed = 0;
    bool finished[n];

    for (int i = 0; i < n; i++)
    {
        finished[i] = false;
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].in_q1 = false;
        processes[i].in_q2 = false;
        processes[i].in_q3 = false;
        processes[i].completed = false;
        processes[i].rem_after_interrupt = 0;
    }

    while (completed < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= time && !finished[i] && !processes[i].completed)
            {

                if (!processes[i].in_q1)
                {
                    enqueue(queue1, &processes[i], &rear1);
                    processes[i].in_q1 = true;
                }

                else if (!processes[i].in_q2 && processes[i].in_q1 && !processes[i].in_q3)
                {
                    enqueue(queue2, &processes[i], &rear2);
                    processes[i].in_q2 = true;
                }

                else if (!processes[i].in_q3 && processes[i].in_q1 && processes[i].in_q2)
                {
                    enqueue(queue3, &processes[i], &rear3);
                    processes[i].in_q3 = true;
                }
            }
        }

        Process *p = NULL;
        int work_for = 0;

        if (front1 != rear1)
        {
            work_for = q1_quantum;
            p = dequeue(queue1, &front1, &rear1);
            printf("Process %d is running in queue 1 at %d.\n", p->id, time);
        }
        else if (front2 != rear2)
        {
            work_for = q2_quantum;
            int time_used = 0;
            bool flag = false;
            p = get_top(queue2, &front2, &rear2);

            if (p->remaining_time <= 0)
            {
                                printf("\t\t\t %d" , p->completed);

                completed++;
                continue;
            }

            while (time_used < work_for)
            {
                bool process_arrives_in_q1 = false;
                for (int i = 0; i < n; i++)
                {
                    if (processes[i].arrival_time == time + time_used && !finished[i] && !processes[i].completed && !processes[i].in_q1)
                    {
                        process_arrives_in_q1 = true;
                        break;
                    }
                }

                if (process_arrives_in_q1)
                {

                    if (front2 != rear2)
                    {
                        flag = true;

                        if (p->remaining_time < time_used)
                        {
                            printf("Process %d is running in queue 2 at %d.\n", p->id, time);
                            time += p->remaining_time;
                            p->remaining_time = 0;
                            p->rem_after_interrupt = 0;
                            completed++;
                            finished[p->id] = true;
                            p->completed = true;
                            dequeue(queue2, &front2, &rear2);
                        }
                        else
                        {
                            printf("Process %d is running in queue 2 at %d.\n", p->id, time);
                            time += time_used;
                            p->remaining_time -= time_used;
                            p->rem_after_interrupt += q2_quantum - time_used;
                        }
                    }
                    break;
                }

                time_used++;
            }
            if (flag)
            {
                continue;
            }

            p = dequeue(queue2, &front2, &rear2);
            printf("Process %d is running in queue 2 at %d.\n", p->id, time);
        }
        else if (front3 != rear3)
        {
            int time_used = 0;
            bool flag = false;
            p = get_top(queue3, &front3, &rear3);

            if (p->remaining_time <= 0)
            {
                completed++;
                continue;
            }

            work_for = p->remaining_time;
            while (time_used < work_for)
            {
                bool process_arrives_in_q1 = false;
                for (int i = 0; i < n; i++)
                {
                    if (processes[i].arrival_time == time + time_used && !finished[i] && !processes[i].in_q1)
                    {
                        process_arrives_in_q1 = true;
                        break;
                    }
                }

                if (process_arrives_in_q1)
                {
                    if (front3 != rear3)
                    {
                        flag = true;
                        printf("Process %d is running in queue 3 at %d.\n", p->id, time);

                        int exec_time = (work_for < time_used) ? work_for : time_used;
                        time += exec_time;
                        p->remaining_time -= exec_time;
                        if (p->remaining_time == 0)
                        {
                            completed++;
                            finished[p->id] = true;
                            p->completed = true;
                            dequeue(queue3, &front3, &rear3);
                        }
                        p->rem_after_interrupt += exec_time;
                    }
                    break;
                }

                time_used++;
            }
            if (flag)
            {
                continue;
            }

            p = dequeue(queue3, &front3, &rear3);
            work_for = p->remaining_time;
            printf("Process %d is running in queue 3 at %d.\n", p->id, time);
        }

        if (p == NULL)
            continue;


        int exec_time = (p->remaining_time < work_for) ? p->remaining_time : work_for;

        if (p->rem_after_interrupt > 0)
        {
            exec_time = p->rem_after_interrupt;
            p->rem_after_interrupt = 0;
        }

        p->remaining_time -= exec_time;
        time += exec_time;

        if (p->remaining_time == 0)
        {
            completed++;
            finished[p->id] = true;
            p->completed = true;
        }
    }

    free(queue1);
    free(queue2);
    free(queue3);
}

int main()
{
    int n;
    scanf("%d", &n);

    Process processes[n];

    for (int i = 0; i < n; i++)
    {
        scanf("%d %d", &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].id = i;
    }

    mlfq(processes, n);
    return 0;
}

