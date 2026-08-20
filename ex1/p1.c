#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t p2, p3;

    // Root process (P1)
    printf("P1 (Root Process)\n");
    printf("PID        : %d\n", getpid());
    printf("Parent PID : %d\n\n", getppid());

    // Create P2
    p2 = fork();

    if (p2 < 0)
    {
        perror("First fork failed");
        return 1;
    }
    else if (p2 == 0)
    {
        // Child process P2
        printf("P2 (Child of P1)\n");
        printf("PID        : %d\n", getpid());
        printf("Parent PID : %d\n\n", getppid());

        // Create P3
        p3 = fork();

        if (p3 < 0)
        {
            perror("Second fork failed");
            return 1;
        }
        else if (p3 == 0)
        {
            // Child process P3
            printf("P3 (Child of P2)\n");
            printf("PID        : %d\n", getpid());
            printf("Parent PID : %d\n", getppid());
        }
        else
        {
            // P2 waits for P3
            wait(NULL);
        }
    }
    else
    {
        // P1 waits for P2
        wait(NULL);
    }

    return 0;
}
