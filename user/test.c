
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
    if (fork() == 0) { //child
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel #A\n");
            exit(1);
        }
        if(channel_put(cd, 43) < 0){
            printf("Failed to put data in channel #B\n");
            exit(1);
        } // Sleeps until cleared
        // Handle error
        sleep(5); //wait for parent to take data before destroying channel
        if (channel_destroy(cd) < 0) {
            printf("Failed to destroy channel #C\n");
            exit(1);
        }
        // Handle error
    } else { //parent
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data from channel #D\n");
            exit(1);
        }
        printf("res=%d\n",data);
        if (channel_take(cd, &data) <0) { //43
            printf("Failed to take data from channel #E\n");
            exit(1);
        }
        printf("res=%d\n",data);
        // Handle error
        if(channel_take(cd, &data)) {// Sleep until child destroys channel
            printf("Failed to take data from channel #F\n");
            exit(1);
        }
        // Handle error
    }
   return 0;
}
