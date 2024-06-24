#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{  
    int n = 3;
    if (argc == 2) {
        n = atoi(argv[1]);
    }
   
    int cid1 = channel_create();
    int cid2 = channel_create();
    if (cid1 < 0 || cid2 < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    
    int count = 0;
    int generator = 2;

    if (fork() == 0) { // Generator process
        while (channel_put(cid1, generator) >= 0) {
            generator++;
        }
        printf("generator done\n");
        exit(0);
    }

    for (int i = 0; i < n; i++) {
        if (fork() == 0) { // Checker process
            for (;;) {
                int candidate;
                if (channel_take(cid1, &candidate) < 0) {
                    printf("Checker done1\n");
                    exit(0);
                }
                if (is_prime(candidate)) {
                    if (channel_put(cid2, candidate) < 0) {
                        printf("Checker done2\n");
                        exit(0);
                    }
                }
            }
        }
    }

    if (fork() == 0) { // Printer process
        while (count < 100) {
            int prime;
            if (channel_take(cid2, &prime) < 0) {
                break;
            }
            printf("prime %d\n", prime);
            count++;
        }
        printf("printer done\n");
        exit(0);
    }

    // Wait for all child processes to finish
    while (wait(0) > 0);
    channel_destroy(cid1);
    channel_destroy(cid2);
    printf("main done\n");
    return 0;
}
