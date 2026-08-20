#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LIMIT 81

int count_vowels(const char *str) {
    int count = 0;
    while (*str) {
        char ch = tolower((unsigned char)*str);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            count++;
        }
        str++;
    }
    return count;
}

int is_palindrome(const char *str) {
    int l = 0;
    int h = strlen(str) - 1;
    while (h > l) {
        if (str[l++] != str[h--]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int pipe_p1_p2[2];
    int pipe_p2_p3[2];
    pid_t p2_pid, p3_pid;

    if (pipe(pipe_p1_p2) == -1 || pipe(pipe_p2_p3) == -1) {
        return 1;
    }

    p2_pid = fork();
    if (p2_pid < 0) {
        return 1;
    }

    if (p2_pid > 0) {
        close(pipe_p1_p2[0]);
        close(pipe_p2_p3[0]);
        close(pipe_p2_p3[1]);

        char input_str[MAX_LIMIT];
        printf("Enter string: ");
        fflush(stdout);
        
        if (fgets(input_str, sizeof(input_str), stdin) != NULL) {
            input_str[strcspn(input_str, "\n")] = '\0';
        } else {
            input_str[0] = '\0';
        }

        write(pipe_p1_p2[1], input_str, strlen(input_str) + 1);
        close(pipe_p1_p2[1]);

        wait(NULL);
    } else { 
        p3_pid = fork();
        if (p3_pid < 0) {
            return 1;
        }

        if (p3_pid > 0) {
            close(pipe_p1_p2[1]);
            close(pipe_p2_p3[0]);

            char read_str[MAX_LIMIT];
            read(pipe_p1_p2[0], read_str, sizeof(read_str));
            close(pipe_p1_p2[0]);

            printf("String read in P2: %s\n", read_str);
            printf("Number of vowels: %d\n", count_vowels(read_str));

            write(pipe_p2_p3[1], read_str, strlen(read_str) + 1);
            close(pipe_p2_p3[1]);

            wait(NULL);
            exit(0);
        } else { 
            close(pipe_p1_p2[0]);
            close(pipe_p1_p2[1]);
            close(pipe_p2_p3[1]);

            char final_str[MAX_LIMIT];
            read(pipe_p2_p3[0], final_str, sizeof(final_str));
            close(pipe_p2_p3[0]);

            if (is_palindrome(final_str)) {
                printf("The string is a palindrome\n");
            } else {
                printf("The string is not a palindrome\n");
            }
            
            exit(0);
        }
    }

    return 0;
}
