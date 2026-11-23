#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static void fill(void)
{
    int *a = malloc(5 * sizeof *a);
    if (a == NULL)
        return;
    for (int i = 0; i <= 5; i++)
        a[i] = i;
    free(a);
}

static void greet(void)
{
    char *s = malloc(16);
    if (s == NULL)
        return;
    strcpy(s, "hello");
    free(s);
    printf("%s\n", s);
}

static void cleanup(void)
{
    char *p = malloc(8);
    free(p);
    free(p);
}

static void remember(void)
{
    char *p = malloc(100);
    if (p == NULL)
        return;
    strcpy(p, "important data");
    printf("%s\n", p);
}

static void scores(void)
{
    int arr[4] = {10, 20, 30, 40};
    int sum = 0;
    for (int i = 0; i <= 4; i++)
        sum += arr[i];
    printf("sum = %d\n", sum);
}

static void counter(void)
{
    int x = INT_MAX;
    for (int i = 0; i < 2; i++)
        x++;
    printf("x = %d\n", x);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s N   (N = 1..6)\n", argv[0]);
        return 1;
    }

    switch (atoi(argv[1])) {
    case 1: fill();     break;
    case 2: greet();    break;
    case 3: cleanup();  break;
    case 4: remember(); break;
    case 5: scores();   break;
    case 6: counter();  break;
    default: printf("unknown test\n"); return 1;
    }
    printf("finished normally\n");
    return 0;
}