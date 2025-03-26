#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 1000
#define MAX_OPER_LEN 1000
#define MAX_MEMBERS 1000

void add(char members[MAX_MEMBERS][MAX_NAME_LEN], int *size, char *new_member, int position)
{
    for (int i = *size; i > position; i--)
    {
        strcpy(members[i], members[i - 1]);
    }
    strcpy(members[position], new_member);
    (*size)++;
}

void delete(char members[MAX_MEMBERS][MAX_NAME_LEN], int *size, char *member_to_remove)
{
    int index = -1;
    for (int i = 0; i < *size; i++)
    {
        if (strcmp(members[i], member_to_remove) == 0)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        for (int i = index; i < *size - 1; i++)
        {
            strcpy(members[i], members[i + 1]);
        }
        (*size)--;
    }
}

void swap(char members[MAX_MEMBERS][MAX_NAME_LEN], char *member1, char *member2, int *size)
{
    if (*size < 1)
    {
        return;
    }

    int index1 = -1, index2 = -1;

    for (int i = 0; i < *size; i++)
    {
        if (strcmp(members[i], member1) == 0)
        {
            index1 = i;
        }
        if (strcmp(members[i], member2) == 0)
        {
            index2 = i;
        }
        if (index1 != -1 && index2 != -1)
            break;
    }

    if (index1 != -1 && index2 != -1)
    {
        char temp[MAX_NAME_LEN];
        strcpy(temp, members[index1]);
        strcpy(members[index1], members[index2]);
        strcpy(members[index2], temp);
    }
}

int main()
{
    int n;
    scanf("%d", &n);

    char members[MAX_MEMBERS][MAX_NAME_LEN];

    for (int i = 0; i < n; i++)
    {
        scanf("%s", members[i]);
    }

    int changes_count;
    scanf("%d", &changes_count);

    for (int i = 0; i < changes_count; i++)
    {
        char operation[MAX_OPER_LEN];

        scanf("%s", operation);

        if (strcmp(operation, "add") == 0)
        {
            char new_member[MAX_NAME_LEN];
            int position;

            scanf("%s", new_member);
            scanf("%d", &position);
            add(members, &n, new_member, position);
        }
        else if (strcmp(operation, "delete") == 0)
        {
            char member_to_remove[MAX_NAME_LEN];

            scanf("%s", member_to_remove);
            delete (members, &n, member_to_remove);
        }
        else if (strcmp(operation, "swap") == 0)
        {
            char member1[MAX_NAME_LEN], member2[MAX_NAME_LEN];

            scanf("%s %s", member1, member2);
            swap(members, member1, member2 , &n);
        }
    }

    for (int i = 0; i < n; i++)
    {
        printf("%s ", members[i]);
    }

    return 0;
}

