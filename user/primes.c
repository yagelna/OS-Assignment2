#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_prime(int n)
{
    if (n <= 1)
    {
        return 0;
    }
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

void task2(int n)
{
    int cid1 = channel_create();
    int cid2 = channel_create();
    if (cid1 < 0 || cid2 < 0)
    {
        printf("Failed to create channel\n");
        exit(1);
    }

    int count = 0;
    int generator = 2;

    if (fork() == 0)
    { // Printer process
        while (count < 100)
        {
            int prime;
            if (channel_take(cid2, &prime) < 0)
            {
                break;
            }
            printf("prime %d\n", prime);
            count++;
        }
        sleep(1);
        printf("Printer done with pid: %d\n", getpid());
        channel_destroy(cid2);
        exit(0);
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            if (fork() == 0)
            { // fork n checkers
                while (1)
                {
                    int candidate;
                    if (channel_take(cid1, &candidate) < 0)
                    {
                        break;
                    }
                    if (is_prime(candidate))
                    {
                        if (channel_put(cid2, candidate) < 0)
                        {
                            break;
                        }
                    }
                }
                channel_destroy(cid1);
                sleep(i);
                printf("Checker %d done with pid: %d\n", i+1, getpid());
                exit(0);
            }
        }
    }

    while (channel_put(cid1, generator) >= 0)
    {
        generator++;
    }
    // Wait for all child processes to finish
    for (int i = 0; i < n + 1; i++)
    {
        wait(0);
    }
    printf("Generator done with pid: %d\n", getpid());
}

int main(int argc, char *argv[])
{
    int n = 3;
    if (argc == 2)
    {
        n = atoi(argv[1]);
    }

    for(;;) {
        task2(n);
        //ask the user if they want to run the program again
        printf("Do you want to run the program again? (y/n): ");
        char ans;
        gets(&ans,5);
        if (ans != 'y')
        {
            break;
        }
    }
    exit(0);
}