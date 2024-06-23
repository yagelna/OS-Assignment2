#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int cd = channel_create();
    printf("1111111\n");
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    channel_put(cd, 42);
    printf("222222222\n");
    int data;
    data = channel_take(cd, &data);
    printf("333333333\n");
    channel_destroy(cd);
    printf("44444444444\n");
    printf("we did it");
    return 0;
    }

