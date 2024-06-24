#include "kernel/types.h"
#include "user/user.h"

#define PRIME_CHECKER_COUNT 3
#define PRIME_NUMBER_LIMIT 100


int
main(int argc, char *argv[])
{
    int checkers_count = PRIME_CHECKER_COUNT;
    if (argc > 1) {
        checkers_count = atoi(argv[1]);
    }

    do {
        int input_cd = channel_create(); 
        int output_cd = channel_create();
        if (input_cd < 0 || output_cd < 0) {
            printf("Failed to create channels\n");
            exit(1); 
        }

        if (fork() == 0) {
            // Printer
            int data;
            for (int i = 0; i < PRIME_NUMBER_LIMIT; i++) {
                if (channel_take(output_cd, &data) < 0) {
                    printf("Failed to take data from channel\n");
                    exit(1); 
                }
                printf("prime %d\n", data);
            }
            // Send signal to checkers to exit
            channel_destroy(output_cd);
            printf("Printer exiting pid: %d\n", getpid());
            exit(0);
        } else {
            // Spawn checkers
            for (int i = 0; i < checkers_count; i++) {
                if (fork() == 0) {
                    int data, j;
                    do {
                        if (channel_take(input_cd, &data) < 0) {
                            //printf("Failed to take data from channel\n");
                            printf("Checker exiting pid: %d\n", getpid());
                            exit(1); 
                        }
                            
                        for (j=2; j < data; j++) {
                            if (data % j == 0) {
                                break;
                            }
                        }
                    } while (data != j || channel_put(output_cd, data) == 0);
                    // Destroy input channel
                    channel_destroy(input_cd);
                    printf("Checker exiting pid: %d\n", getpid());
                    exit(0);
                }
            }

            // Send data to checkers
            int data = 2;
            while (channel_put(input_cd, data) == 0) {
                data++;
            }
            // Wait for all checkers and printer to finish
            for (int i = 0; i < checkers_count + 1; i++) {
                wait(0);
            }

            printf("All checkers and printer finished\n");
            printf("Generator exiting pid: %d\n", getpid());
        }
        printf("Start again? (y/n):");
        char c;
        gets(&c, 3);
        if (c != 'y') {
            exit(0);
        }
    } while (1);
}