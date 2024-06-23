
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
        }
    else{
        printf("Channel created with id [%d]\n",cd);
    }
    if (fork() == 0) {
        if (channel_put(cd, 42) < 0) {
        printf("Failed to put data in channel\n");
        exit(1);
        }
        channel_put(cd, 43); // Sleeps until cleared
        // Handle error
        channel_destroy(cd);
        // Handle error
    } else {
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data from channel\n");
            exit(1);
            }
        printf("1)data is:%d \n",data);
        int res = channel_take(cd, &data); // 43
        printf("2)data is:%d with result %d\n",data, res);
        // Handle error
        res = channel_take(cd, &data); // Sleep until child destroys channel
        printf("3)data is:%d with result %d\n",data, res);
        // Handle error
    }
   return 0;
}

