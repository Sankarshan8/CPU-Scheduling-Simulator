#include <stdio.h>

#define MAX 50

struct Process {
    int pid;
    int at;
    int bt;
    int priority;
    int ct;
    int tat;
    int wt;
    int remaining;
};

/* ---------- Utility Functions ---------- */

void reset(struct Process p[], int n) {
    for (int i = 0; i < n; i++) {
        p[i].ct = 0;
        p[i].tat = 0;
        p[i].wt = 0;
        p[i].remaining = p[i].bt;
    }
}

void calculate(struct Process p[], int n) {
    float avgWT = 0, avgTAT = 0;

    for (int i = 0; i < n; i++) {
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        avgWT += p[i].wt;
        avgTAT += p[i].tat;
    }

    printf("\nProcess\tAT\tBT\tPriority\tCT\tTAT\tWT\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
               p[i].pid, p[i].at, p[i].bt,
               p[i].priority, p[i].ct,
               p[i].tat, p[i].wt);
    }

    printf("\nAverage Waiting Time    : %.2f\n", avgWT / n);
    printf("Average Turnaround Time : %.2f\n", avgTAT / n);
}

void gantt(int order[], int times[], int count) {
    printf("\nGantt Chart:\n");

    for (int i = 0; i < count; i++)
        printf("--------");
    printf("-\n");

    for (int i = 0; i < count; i++)
        printf("|  P%d  ", order[i]);
    printf("|\n");

    for (int i = 0; i < count; i++)
        printf("--------");
    printf("-\n");

    printf("%d", times[0]);

    for (int i = 1; i <= count; i++)
        printf("\t%d", times[i]);

    printf("\n");
}

/* ---------- FCFS ---------- */

void fcfs(struct Process p[], int n) {
    reset(p, n);

    int order[MAX], times[MAX + 1];
    int count = 0;
    int time = 0;

    /* Sort according to arrival time */
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[i].at > p[j].at) {
                struct Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    for (int i = 0; i < n; i++) {

        if (time < p[i].at)
            time = p[i].at;

        order[count] = p[i].pid;
        times[count] = time;

        time += p[i].bt;
        p[i].ct = time;

        count++;
    }

    times[count] = time;

    printf("\n========== FCFS ==========\n");
    gantt(order, times, count);
    calculate(p, n);
}

/* ---------- SJF ---------- */

void sjf(struct Process p[], int n) {
    reset(p, n);

    int completed = 0;
    int time = 0;
    int order[MAX], times[MAX + 1];
    int count = 0;

    while (completed < n) {

        int index = -1;
        int shortest = 99999;

        for (int i = 0; i < n; i++) {
            if (p[i].ct == 0 && p[i].at <= time) {

                if (p[i].bt < shortest) {
                    shortest = p[i].bt;
                    index = i;
                }
            }
        }

        if (index == -1) {
            time++;
            continue;
        }

        order[count] = p[index].pid;
        times[count] = time;

        time += p[index].bt;
        p[index].ct = time;

        count++;
        completed++;
    }

    times[count] = time;

    printf("\n========== SJF ==========\n");
    gantt(order, times, count);
    calculate(p, n);
}

/* ---------- Priority Scheduling ---------- */

void priorityScheduling(struct Process p[], int n) {
    reset(p, n);

    int completed = 0;
    int time = 0;
    int order[MAX], times[MAX + 1];
    int count = 0;

    while (completed < n) {

        int index = -1;
        int bestPriority = 99999;

        for (int i = 0; i < n; i++) {

            if (p[i].ct == 0 && p[i].at <= time) {

                /* Smaller priority number = higher priority */
                if (p[i].priority < bestPriority) {
                    bestPriority = p[i].priority;
                    index = i;
                }
            }
        }

        if (index == -1) {
            time++;
            continue;
        }

        order[count] = p[index].pid;
        times[count] = time;

        time += p[index].bt;
        p[index].ct = time;

        count++;
        completed++;
    }

    times[count] = time;

    printf("\n========== PRIORITY ==========\n");
    gantt(order, times, count);
    calculate(p, n);
}

/* ---------- Round Robin ---------- */

void roundRobin(struct Process p[], int n, int quantum) {
    reset(p, n);

    int queue[MAX * 10];
    int front = 0, rear = 0;

    int visited[MAX] = {0};

    int order[MAX * 10];
    int times[MAX * 10 + 1];
    int count = 0;

    int time = 0;
    int completed = 0;

    /* Find first arriving process */
    int first = 0;

    for (int i = 1; i < n; i++) {
        if (p[i].at < p[first].at)
            first = i;
    }

    time = p[first].at;

    queue[rear++] = first;
    visited[first] = 1;

    while (completed < n) {

        if (front == rear) {

            for (int i = 0; i < n; i++) {
                if (!visited[i] && p[i].remaining > 0) {
                    time = p[i].at;
                    queue[rear++] = i;
                    visited[i] = 1;
                    break;
                }
            }

            continue;
        }

        int index = queue[front++];

        order[count] = p[index].pid;
        times[count] = time;

        int execution;

        if (p[index].remaining > quantum)
            execution = quantum;
        else
            execution = p[index].remaining;

        time += execution;
        p[index].remaining -= execution;

        /* Add newly arrived processes */
        for (int i = 0; i < n; i++) {
            if (!visited[i] &&
                p[i].at <= time &&
                p[i].remaining > 0) {

                queue[rear++] = i;
                visited[i] = 1;
            }
        }

        if (p[index].remaining > 0) {
            queue[rear++] = index;
        } else {
            p[index].ct = time;
            completed++;
        }

        count++;
    }

    times[count] = time;

    printf("\n========== ROUND ROBIN ==========\n");
    printf("Time Quantum: %d\n", quantum);

    gantt(order, times, count);
    calculate(p, n);
}

/* ---------- Main ---------- */

int main() {

    struct Process p[MAX];
    int n;
    int choice;
    int quantum;

    printf("========================================\n");
    printf("     CPU SCHEDULING ALGORITHM SIMULATOR\n");
    printf("========================================\n");

    printf("\nEnter number of processes: ");
    scanf("%d", &n);

    if (n <= 0 || n > MAX) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    printf("\nEnter process details:\n");
    printf("(Smaller priority number = higher priority)\n\n");

    for (int i = 0; i < n; i++) {

        p[i].pid = i + 1;

        printf("P%d Arrival Time: ", i + 1);
        scanf("%d", &p[i].at);

        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &p[i].bt);

        printf("P%d Priority: ", i + 1);
        scanf("%d", &p[i].priority);

        printf("\n");
    }

    while (1) {

        printf("\n========================================\n");
        printf("              MENU\n");
        printf("========================================\n");
        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. Priority Scheduling\n");
        printf("4. Round Robin\n");
        printf("5. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                fcfs(p, n);
                break;

            case 2:
                sjf(p, n);
                break;

            case 3:
                priorityScheduling(p, n);
                break;

            case 4:
                printf("Enter Time Quantum: ");
                scanf("%d", &quantum);

                if (quantum <= 0) {
                    printf("Invalid time quantum.\n");
                } else {
                    roundRobin(p, n, quantum);
                }
                break;

            case 5:
                printf("\nThank you!\n");
                return 0;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }

    return 0;
}
