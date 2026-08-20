#include <stdio.h>
#include <stdlib.h>
#define MAX 100

/* ---- Globals (see note above) ---- */
int n; /* number of processes */
int priority[MAX]; /* priority[i] for processlst[i] */

/* ---- Prototype Declarations (as specified) ---- */
void fcfs(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime);
void sjfs(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime);
void srtf(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime);
void prioritySchedule(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime);
void roundRobin(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime, int timequantum);

/* ---- Helper to print a result table ---- */
static void printTable(int *processlst, int *bursttime, int *arrivaltime, int *completion, int *waiting, int *turnaround)
{
    printf("\n+------------+------------+------------+------------+------------+------------+\n");
    printf("| %-10s | %-10s | %-10s | %-10s | %-10s | %-10s |\n", "Process", "Arrival", "Burst", "Completion", "Waiting", "Turnaround");
    printf("+------------+------------+------------+------------+------------+------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-9d | %-10d | %-10d | %-10d | %-10d | %-10d |\n", 
               processlst[i], arrivaltime[i], bursttime[i], completion[i], waiting[i], turnaround[i]);
    }
    printf("+------------+------------+------------+------------+------------+------------+\n");
}

/* ==============================
* 1. FCFS - First Come First Serve
* ============================== */
void fcfs(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime)
{
    int idx[MAX];
    int completion[MAX], waiting[MAX], turnaround[MAX];
    for (int i = 0; i < n; i++) idx[i] = i;
    
    /* sort indices by arrival time (stable insertion sort) */
    for (int i = 1; i < n; i++) {
        int key = idx[i], j = i - 1;
        while (j >= 0 && arrivaltime[idx[j]] > arrivaltime[key]) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
    
    int time = 0;
    int totalWT = 0, totalTAT = 0;
    for (int k = 0; k < n; k++) {
        int i = idx[k];
        if (time < arrivaltime[i]) time = arrivaltime[i];
        time += bursttime[i];
        completion[i] = time;
        turnaround[i] = completion[i] - arrivaltime[i];
        waiting[i] = turnaround[i] - bursttime[i];
        totalWT += waiting[i];
        totalTAT += turnaround[i];
    }
    
    printf("\n>>> FCFS SCHEDULING RESULTS <<<\n");
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

/* ==============================
* 2. SJF - Shortest Job First (Non-preemptive)
* ============================== */
void sjfs(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime)
{
    int completion[MAX], waiting[MAX], turnaround[MAX], done[MAX];
    int remainingBurst[MAX];
    for (int i = 0; i < n; i++) { done[i] = 0; remainingBurst[i] = bursttime[i]; }
    int time = 0, completed = 0;
    int totalWT = 0, totalTAT = 0;
    
    while (completed < n) {
        int chosen = -1;
        int minBurst = 1e9;
        for (int i = 0; i < n; i++) {
            if (!done[i] && arrivaltime[i] <= time && bursttime[i] < minBurst) {
                minBurst = bursttime[i];
                chosen = i;
            }
        }
        if (chosen == -1) {
            /* no process has arrived yet, jump time forward */
            time++;
            continue;
        }
        time += bursttime[chosen];
        completion[chosen] = time;
        turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
        waiting[chosen] = turnaround[chosen] - bursttime[chosen];
        totalWT += waiting[chosen];
        totalTAT += turnaround[chosen];
        done[chosen] = 1;
        completed++;
    }
    
    printf("\n>>> SJF (NON-PREEMPTIVE) SCHEDULING RESULTS <<<\n");
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

/* ==============================
* 3. SRTF - Shortest Remaining Time First (Preemptive SJF)
* ============================== */
void srtf(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime)
{
    int remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];
    for (int i = 0; i < n; i++) remaining[i] = bursttime[i];
    int completed = 0, time = 0;
    int totalWT = 0, totalTAT = 0;
    
    /* find max arrival + total burst to bound simulation */
    int maxArrival = 0, totalBurst = 0;
    for (int i = 0; i < n; i++) {
        if (arrivaltime[i] > maxArrival) maxArrival = arrivaltime[i];
        totalBurst += bursttime[i];
    }
    int limit = maxArrival + totalBurst + 1;
    
    while (completed < n && time < limit) {
        int chosen = -1;
        int minRem = 1e9;
        for (int i = 0; i < n; i++) {
            if (arrivaltime[i] <= time && remaining[i] > 0 && remaining[i] < minRem) {
                minRem = remaining[i];
                chosen = i;
            }
        }
        if (chosen == -1) {
            time++;
            continue;
        }
        remaining[chosen]--;
        time++;
        if (remaining[chosen] == 0) {
            completion[chosen] = time;
            turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
            waiting[chosen] = turnaround[chosen] - bursttime[chosen];
            totalWT += waiting[chosen];
            totalTAT += turnaround[chosen];
            completed++;
        }
    }
    
    printf("\n>>> SRTF (PREEMPTIVE SJF) SCHEDULING RESULTS <<<\n");
    printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

/* ==============================
* 4. Priority Scheduling (Non-preemptive, lower number = higher priority)
* ============================== */
void prioritySchedule(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime)
{
    int completion[MAX], waiting[MAX], turnaround[MAX], done[MAX];
    for (int i = 0; i < n; i++) done[i] = 0;
    int time = 0, completed = 0;
    int totalWT = 0, totalTAT = 0;
    
    while (completed < n) {
        int chosen = -1;
        int bestPriority = 1e9;
        for (int i = 0; i < n; i++) {
            if (!done[i] && arrivaltime[i] <= time && priority[i] < bestPriority) {
                bestPriority = priority[i];
                chosen = i;
            }
        }
        if (chosen == -1) {
            time++;
            continue;
        }
        time += bursttime[chosen];
        completion[chosen] = time;
        turnaround[chosen] = completion[chosen] - arrivaltime[chosen];
        waiting[chosen] = turnaround[chosen] - bursttime[chosen];
        totalWT += waiting[chosen];
        totalTAT += turnaround[chosen];
        done[chosen] = 1;
        completed++;
    }
    
    printf("\n>>> PRIORITY SCHEDULING RESULTS <<<\n");
    printf("+------------+------------+------------+------------+------------+------------+------------+\n");
    printf("| %-10s | %-10s | %-10s | %-10s | %-10s | %-10s | %-10s |\n", "Process", "Arrival", "Burst", "Priority", "Completion", "Waiting", "Turnaround");
    printf("+------------+------------+------------+------------+------------+------------+------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-9d | %-10d | %-10d | %-10d | %-10d | %-10d | %-10d |\n", 
               processlst[i], arrivaltime[i], bursttime[i], priority[i], completion[i], waiting[i], turnaround[i]);
    }
    printf("+------------+------------+------------+------------+------------+------------+------------+\n");
    *awtime = totalWT / n;
    *atattime = totalTAT / n;
}

/* ==============================
* 5. Round Robin
* ============================== */
void roundRobin(int *processlst, int *bursttime, int *arrivaltime, int *awtime, int *atattime, int timequantum)
{
    int remaining[MAX], completion[MAX], waiting[MAX], turnaround[MAX];
    int queue[MAX * 50], front = 0, rear = 0;
    int inQueue[MAX] = {0};
    for (int i = 0; i < n; i++) remaining[i] = bursttime[i];
    
    /* sort process indices by arrival time to enqueue correctly */
    int idx[MAX];
    for (int i = 0; i < n; i++) idx[i] = i;
    for (int i = 1; i < n; i++) {
        int key = idx[i], j = i - 1;
        while (j >= 0 && arrivaltime[idx[j]] > arrivaltime[key]) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = key;
    }
    
    int time = 0, completed = 0, ptr = 0;
    while (ptr < n && arrivaltime[idx[ptr]] <= time) {
        queue[rear++] = idx[ptr];
        inQueue[idx[ptr]] = 1;
        ptr++;
    }
    if (rear == front) {
        time = arrivaltime[idx[0]];
        while (ptr < n && arrivaltime[idx[ptr]] <= time) {
            queue[rear++] = idx[ptr];
            inQueue[idx[ptr]] = 1;
            ptr++;
        }
    }
    
    int totalWT = 0, totalTAT = 0;
    while (completed < n) {
        int i = queue[front++];
        int slice = (remaining[i] < timequantum) ? remaining[i] : timequantum;
        time += slice;
        remaining[i] -= slice;
        
        while (ptr < n && arrivaltime[idx[ptr]] <= time) {
            queue[rear++] = idx[ptr];
            inQueue[idx[ptr]] = 1;
            ptr++;
        }
        if (remaining[i] > 0) {
            queue[rear++] = i;
        } else {
            completion[i] = time;
            turnaround[i] = completion[i] - arrivaltime[i];
            waiting[i] = turnaround[i] - bursttime[i];
            totalWT += waiting[i];
            totalTAT += turnaround[i];
            completed++;
        }
        if (front == rear && completed < n && ptr < n) {
            time = arrivaltime[idx[ptr]];
        while (ptr < n && arrivaltime[idx[ptr]] <= time) {
            queue[rear++] = idx[ptr];
            inQueue[idx[ptr]] = 1;
            ptr++;
        }
    }
}

printf("\n>>> ROUND ROBIN SCHEDULING RESULTS (Time Quantum = %d) <<<\n", timequantum);
printTable(processlst, bursttime, arrivaltime, completion, waiting, turnaround);
*awtime = totalWT / n;
*atattime = totalTAT / n;
}

/* ==============================
* MAIN - Menu driven driver program
* ============================== */
int main(void)
{
    int processlst[MAX], bursttime[MAX], arrivaltime[MAX];
    int awtime, atattime, choice, tq;

    printf("=========================================\n");
    printf("        CPU SCHEDULING SIMULATOR         \n");
    printf("=========================================\n\n");

    printf("Enter total number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        processlst[i] = i + 1;
        printf("-> [P%d] Enter Arrival Time & Burst Time: ", processlst[i]);
        scanf("%d %d", &arrivaltime[i], &bursttime[i]);
    }

    do {
        printf("\n=========================================\n");
        printf("          CPU SCHEDULING MENU            \n");
        printf("=========================================\n");
        printf(" 1. FCFS\n");
        printf(" 2. SJF (Non-preemptive)\n");
        printf(" 3. SRTF (Preemptive SJF)\n");
        printf(" 4. Priority Scheduling\n");
        printf(" 5. Round Robin\n");
        printf(" 0. Exit Program\n");
        printf("-----------------------------------------\n");
        printf("Enter option choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                fcfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
                printf("\n>> Summary Stats:\n");
                printf("   Average Waiting Time    = %d ms\n", awtime);
                printf("   Average Turnaround Time = %d ms\n", atattime);
                break;
            case 2:
                sjfs(processlst, bursttime, arrivaltime, &awtime, &atattime);
                printf("\n>> Summary Stats:\n");
                printf("   Average Waiting Time    = %d ms\n", awtime);
                printf("   Average Turnaround Time = %d ms\n", atattime);
                break;
            case 3:
                srtf(processlst, bursttime, arrivaltime, &awtime, &atattime);
                printf("\n>> Summary Stats:\n");
                printf("   Average Waiting Time    = %d ms\n", awtime);
                printf("   Average Turnaround Time = %d ms\n", atattime);
                break;
            case 4:
                for (int i = 0; i < n; i++) {
                    printf("-> Assign Priority for P%d (lower values indicate higher priority): ", processlst[i]);
                    scanf("%d", &priority[i]);
                }
                prioritySchedule(processlst, bursttime, arrivaltime, &awtime, &atattime);
                printf("\n>> Summary Stats:\n");
                printf("   Average Waiting Time    = %d ms\n", awtime);
                printf("   Average Turnaround Time = %d ms\n", atattime);
                break;
            case 5:
                printf("-> Input desired Time Quantum duration: ");
                scanf("%d", &tq);
                roundRobin(processlst, bursttime, arrivaltime, &awtime, &atattime, tq);
                printf("\n>> Summary Stats:\n");
                printf("   Average Waiting Time    = %d ms\n", awtime);
                printf("   Average Turnaround Time = %d ms\n", atattime);
                break;
            case 0:
                printf("\nShutting down simulator process...\n");
                break;
            default:
                printf("\n[!] Input choice invalid. Please choose a valid menu item.\n");
        }
    } while (choice != 0);

    return 0;
}

