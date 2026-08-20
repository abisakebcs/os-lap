#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // 1. Root process p1 prints first
    printf("p1: pid=%d parent=%d\n", getpid(), getppid());
    
    pid_t pid = fork();
    if (pid == 0) {
        // 2. Child process p2 prints
        printf("p2: pid=%d parent=%d\n", getpid(), getppid());
        
        pid = fork();
        if (pid == 0) {
            // 3. Child process p4 prints
            printf("p4: pid=%d parent=%d\n", getpid(), getppid());
            
            pid = fork();
            if (pid == 0) {
                // 4. Child process p6 prints
                printf("p6: pid=%d parent=%d\n", getpid(), getppid());
            } else {
                wait(NULL); // p4 waits for p6 to finish
            }
        } else {
            wait(NULL); // p2 waits for p4 to finish
        }
    } else if (pid > 0) {
        // p1 explicitly waits for p2's branch (p2 -> p4 -> p6) to entirely finish
        wait(NULL); 
        
        // 5. Back in p1, fork the second child p3 only AFTER p2 branch is done
        pid = fork();
        if (pid == 0) {
            // 6. Child process p3 prints
            printf("p3: pid=%d parent=%d\n", getpid(), getppid());
            
            pid = fork();
            if (pid == 0) {
                // 7. Child process p5 prints
                printf("p5: pid=%d parent=%d\n", getpid(), getppid());
                
                pid = fork();
                if (pid == 0) {
                    // 8. Child process p7 prints
                    printf("p7: pid=%d parent=%d\n", getpid(), getppid());
                } else { 
                    // p5 waits for p7 to finish completely BEFORE creating p8
                    wait(NULL); 
                    
                    pid = fork();
                    if (pid == 0) {
                        // 9. Child process p8 prints
                        printf("p8: pid=%d parent=%d\n", getpid(), getppid());
                    } else {
                        wait(NULL); // p5 waits for p8 to finish
                    }
                }
            } else {
                wait(NULL); // p3 waits for p5 to finish
            }
        } else {
            wait(NULL); // p1 waits for p3 to finish
        }
    }
    return 0;
}
