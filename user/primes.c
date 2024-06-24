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
    if(argc == 2) {
        n = atoi(argv[1]);
    }
   
    int cid1 = channel_create();
    int cid2 = channel_create();
    int count = 0;
    int generator = 2;
    if (cid1 < 0 || cid2 < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    for(;;){
    if (fork() == 0) { //generator

            while (channel_put(cid1, generator) >= 0){
                generator++;
            }
    } else {
        while (n != 0) {
            n--;
            if (fork() == 0) { //filter
                
                for (;;) {
                
                    int candidate;
                    if (channel_take(cid1, &candidate) < 0) {
                        exit(0);
                    }
                    if (is_prime(candidate))
                    {
                        if (channel_put(cid2, candidate) < 0) {
                            exit(0);
                        }
                    }
                }
            }
        }
        if (fork()==0) // printer
        {
            int prime;
            while (channel_take(cid2, &prime) >= 0 && count < 100) {
                
                printf("prime %d\n", prime);
                count++;
            }

        }
        }
        return 0;
    }
}


