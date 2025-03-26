#include <stdio.h>

void take_input(int n, int i, long long s)
{

    if (i > 0)
    {
        printf("Sum until number %d: %lld\n", i, s);
    }

    int inp;
    if (i < n)
    {
        scanf("%d", &inp);
        take_input(n, i + 1, s + inp);
    }
}

int main()
{
    int n;
    printf("Enter the number of inputs: ");
    scanf("%d", &n);

    take_input(n, 0, 0);

    return 0;
}
