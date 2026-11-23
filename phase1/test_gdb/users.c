#include <stdio.h>
#include <string.h>

struct user {
    char name[16];
    int  age;
};

static struct user users[] = {
    {"alice", 30},
    {"bob",   25},
    {"carol", 41},
};

static struct user *find_user(const char *name)
{
    for (size_t i = 0; i < sizeof users / sizeof users[0]; i++)
        if (strcmp(users[i].name, name) == 0)
            return &users[i];
    return NULL;
}

static int total_age(const char **names, size_t n)
{
    int sum = 0;

    for (size_t i = 0; i < n; i++) {
        struct user *u = find_user(names[i]);
        if(u != NULL)
            sum += u->age;
    }
    return sum;
}

int main(void)
{
    const char *names[] = {"alice", "bob", "dave", "carol"};

    printf("total age: %d\n", total_age(names, 4));
    return 0;
}