#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX 10  // process 최대 개수
#define MAX_IO_cnt 5    // 최대 I/O 발생 횟수
#define alg_num 6  // cpu scheduling 알고리즘 개수


typedef struct {
    int PID;
    int arrival;
    int cpu_burst;
    int priority;

    int IO_request[MAX_IO_cnt]; // I/O 발생 시점 
    int IO_burst[MAX_IO_cnt];   // I/O 실행 시간
    int IO_cnt;                 // I/O 발생 횟수수

} process;


typedef struct {

    char process_name[3];
    int start;
    int end;

}ganttseg;


int process_num;

int total_wait[alg_num];
int total_turnaround[alg_num];

process job_queue[MAX];
process ready_queue[MAX];

ganttseg gantt[50];
int gantt_cnt = 0;


void Create_Process(void);
void Schedule(void);
void Evaluation(void);

void FCFS(void);
void SJF(void);
void Priority(void);
void RR(void);
void Preemtive_SJF(void);
void Preemtive_Priority(void);

void print_process(void);   // process의 상태 출력
void print_Gantt(char*);     //Gantt chart 출력
void write_Gantt(int, int, int, int, int);     // 프로세스의 실행이 끝날 때마다 Gantt chart에 기록을 남김
void print_process_wait_turnaroud(char*, int*, int*);
void job_schedulng(void);   // job_queue에 있는 process들을 ready_queue로 옮기는 함수
void sort_request(int);    // I/O request 정렬


int main(void) {

    Create_Process();
    Schedule();
    Evaluation();

    return 0;
}

void Create_Process(void) {

    printf("Enter the number of process(1 ~ %d): ", MAX);
    scanf("%d", &process_num);  // process의 개수 입력 받음    

    for (int i = 0; i < process_num; i++) {

        printf("Enter PID of %dth process: ", i + 1); // 각 process의 PID를 입력받음
        scanf("%d", &job_queue[i].PID);

        srand(time(NULL));
        job_queue[i].arrival = rand() % (MAX * 2);    // 도착 시간: 0 ~ 19
        job_queue[i].cpu_burst = rand() % (MAX * 2) + 1;   // CPU burst 시간: 1 ~ 20
        job_queue[i].priority = rand() % process_num + 1;    // 우선순위: 1 ~ process_num

        if (job_queue[i].cpu_burst > 1) {

            job_queue[i].IO_cnt = (rand() % (job_queue[i].cpu_burst)) % (MAX_IO_cnt + 1);  // 0 ~ (cpu_burst-1 and MAX_IO_cnt)번 I/O 요청, I/O 요청이 cpu burst를 초과하면 안됨

            for (int j = 0; j < job_queue[i].IO_cnt; ) {

                int request = (rand() % (job_queue[i].cpu_burst - 1)) + 1;  // 1 ~ (cpu_burst-1) 범위의 I/O request time 랜덤 생성
                int duplicate = 0;

                // I/O 발생의 중복 확인
                for (int k = 0; k < j; k++) {

                    if (request == job_queue[i].IO_request[k]) {
                        duplicate = 1;
                        break;
                    }
                }

                if (!duplicate) {
                    job_queue[i].IO_request[j] = request;
                    job_queue[i].IO_burst[j] = (rand() % 5) + 1;    // 1~5 사이의 I/O burst time
                    j++;    // I/O 발생이 중복된 경우가 없을 경우 j를 증가 시킴
                }

            }

            sort_request(i); // 랜덤으로 발생한 I/O request 순서대로 정렬

        }
        else {
            job_queue[i].IO_cnt = 0;
        }
    }

    print_process();

    return;

}

void sort_request(int idx) {
     
    int tmp1 = 0;
    int tmp2 = 0;

    for (int i = 0; i < job_queue[idx].IO_cnt - 1; i++) {
        for (int j = i + 1; j < job_queue[idx].IO_cnt; j++) {

            if (job_queue[idx].IO_request[i] > job_queue[idx].IO_request[j]) {

                // request 정렬
                tmp1 = job_queue[idx].IO_request[i];
                job_queue[idx].IO_request[i] = job_queue[idx].IO_request[j];
                job_queue[idx].IO_request[j] = tmp1;

                // request 정렬 시, I/O burst도 같이 정렬
                tmp2 = job_queue[idx].IO_burst[i];
                job_queue[idx].IO_burst[i] = job_queue[idx].IO_burst[j];
                job_queue[idx].IO_burst[j] = tmp2;

            }
        }
    }


}


void print_process(void) {

    printf("+-----------+---------+-----------+---------+------------+--------------------+------------------+\n");
    printf("|  Process  |   PID   |  Arrival  |  Burst  |  Priority  |  I/O Request Time  |  I/O Burst Time  |\n");
    printf("+-----------+---------+-----------+---------+------------+--------------------+------------------+\n");


    for (int i = 0; i < process_num; i++) {

        printf("|    P%-2d    |   %-3d   |    %-3d    |   %-3d   |     %-3d    |", i, job_queue[i].PID, job_queue[i].arrival, job_queue[i].cpu_burst, job_queue[i].priority);

        // I/O request 출력
        if (job_queue[i].IO_cnt) {

            for (int j = 0; j < job_queue[i].IO_cnt; j++)
                printf("  %-2d", job_queue[i].IO_request[j]);
            for (int j = job_queue[i].IO_cnt; j < MAX_IO_cnt; j++)
                printf("    ");
            printf("|");

        }
        else {

            printf("                    |");
        }

        // I/O burst time 출력
        if (job_queue[i].IO_cnt) {

            for(int j = 0; j < job_queue[i].IO_cnt; j++)
                printf("  %d", job_queue[i].IO_burst[j]);
            for(int j = job_queue[i].IO_cnt; j < MAX_IO_cnt; j++)
                printf("   ");
            printf("   |\n");

        }
        else {

            printf("                  |\n");
        }

    }

    printf("+-----------+---------+-----------+---------+------------+--------------------+------------------+\n");

    return;

}


void Schedule(void) {

    job_schedulng();
    FCFS();

    job_schedulng();
    SJF();

    job_schedulng();
    Priority();

    job_schedulng();
    RR();

    job_schedulng();
    Preemtive_SJF();

    job_schedulng();
    Preemtive_Priority();

    return;

}


void job_schedulng(void) {

    for (int i = 0; i < process_num; i++)
        ready_queue[i] = job_queue[i];

    return;

}


void write_Gantt(int idx, int time, int exec_time, int preemtive, int start_current) {

    if (idx == -1)
        sprintf(gantt[gantt_cnt].process_name, "--");
    else
        sprintf(gantt[gantt_cnt].process_name, "P%d", idx);

    if (preemtive)
        gantt[gantt_cnt].start = start_current;
    else
        gantt[gantt_cnt].start = time - exec_time;

    gantt[gantt_cnt].end = time;
    gantt_cnt++;

    return;

}


void print_Gantt(char* alg) {

    const int scale1 = 4;   // Gantt chart의 각 process 출력 크기 5로 고정
    const int scale2 = 6;    // Gantt chart의 각 process 출력 크기 5로 고정
    printf("\n%s Gantt Chart:\n", alg);


    // Gantt chart 상단 바 출력
    printf("+");

    for (int i = 0; i < gantt_cnt; i++) {

        int exec = gantt[i].end - gantt[i].start;

        if (exec < 10) {
            for (int j = 0; j < scale1; j++)
                printf("-");
        }
        else {

            for (int j = 0; j < scale2; j++)
                printf("-");

        }
        printf("+");
    }

    // Gantt chart 프로세스 이름 출력

    printf("\n|");

    for (int i = 0; i < gantt_cnt; i++) {

        int exec = gantt[i].end - gantt[i].start;

        if (exec < 10) {
                printf(" ");
        }
        else {
                printf("  ");

        }


        printf("%s", gantt[i].process_name);

        if (exec < 10) {
            printf(" ");
        }
        else {
            printf("  ");

        }

        printf("|");


    }


    // Gantt chart 하단 바 출력
    printf("\n+");

    for (int i = 0; i < gantt_cnt; i++) {

        int exec = gantt[i].end - gantt[i].start;

        if (exec < 10) {
            for (int j = 0; j < scale1; j++)
                printf("-");
        }
        else {

            for (int j = 0; j < scale2; j++)
                printf("-");

        }
        printf("+");
    }

    // Gantt chart 시간 출력
    printf("\n0");

    for (int i = 0; i < gantt_cnt; i++) {

        int exec = gantt[i].end - gantt[i].start;
        
        if (exec < 10) {
            for (int j = 0; j < scale1; j++)
                printf(" ");
        }
        else {

            for (int j = 0; j < scale2; j++)
                printf(" ");

        }
        printf("%d", gantt[i].end);
    }

    return;

}

void print_process_wait_turnaroud(char* alg, int* process_wait, int* process_turnaround) {

    printf("\n-----------------------------------------------------------\n");

    printf("\n%s's process waiting time & turnaroud time:\n", alg);

    printf("+-----------+------------------+------------------+\n");
    printf("|  Process  |   waiting time   |  turnaroud time  |\n");
    printf("+-----------+------------------+------------------+\n");


    for (int i = 0; i < process_num; i++) {

        printf("|    P%-2d    |       %-3d        |       %-3d        |\n", i, process_wait[i], process_turnaround[i]);
        printf("+-----------+------------------+------------------+\n");

    }


    return;

}

void FCFS(void) {

    int time = 0;
    int finished = 0;

    total_wait[0] = 0;
    total_turnaround[0] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];   // 각 프로세스의 현재 IO 처리 인덱스
    int wait_until[MAX]; // I/O 끝나는 시점 저장
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };  // 각 프로세스의 waiting time
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time

    gantt_cnt = 0;


    for (int i = 0; i < process_num; i++) {

        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;

    }

    while (finished < process_num) {

        int idx = -1;

        for (int i = 0; i < process_num; i++) {

            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue;  // I/O 중이면 건너뜀
            if (ready_queue[i].arrival > time) continue;    // process가 time 시간 내에 도착해 있어야 처리 가능

            if (idx == -1 || (ready_queue[i].arrival < ready_queue[idx].arrival))
                idx = i;
        }

        // 모든 process가 time 시간 내에 있지 않을 경우
        if (idx == -1) {
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }

        int exec_time = remaining_burst[idx];  // 현재 burst 전체를 실행할 수 있다고 가정

        // 다음 I/O 요청이 언제 오는지 계산하여 cpu 실행 시간 조절
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int io_request = ready_queue[idx].IO_request[io_index[idx]];    // I/O 요청 발생 시점
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];   // 현재까지 실행된 CPU 시간
            if (io_request - progress < exec_time) {
                exec_time = io_request - progress;
            }
        }

        if (exec_time > 0) {
            time += exec_time;
            remaining_burst[idx] -= exec_time;
            write_Gantt(idx, time, exec_time, 0, 0);
        }

        // process가 완전히 끝났을 때 해당 process의 waiting time과 turnaround time 계산
        if (remaining_burst[idx] == 0) {

            process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // 각 process의 waiting time 계산
            total_wait[0] += process_wait[idx];     // 각 process의 waiting time을 더해 total waiting time을 구함
            process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
            total_turnaround[0] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산
            finished++;
            continue;
        }

        // 지금 실행 중인 process가 I/O 요청 시점에 도달했는지 확인
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (progress == ready_queue[idx].IO_request[io_index[idx]]) {

                process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // I/O 요청 발생 시 해당 process의 waiting time

                wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                ready_queue[idx].arrival = wait_until[idx];     // I/O 완료 후 arrival 시간 갱신
                io_index[idx]++;

                continue;  // I/O 처리 후 다시 ready 상태로 돌아감
            }
        }


    }

    print_process_wait_turnaroud("FCFS", process_wait, process_turnaround);

    print_Gantt("FCFS");

    return;

}

void SJF(void) {

    int time = 0;
    int finished = 0;

    total_wait[1] = 0;
    total_turnaround[1] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];   // 각 프로세스의 현재 IO 처리 인덱스
    int wait_until[MAX]; // I/O 끝나는 시점 저장
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };  // 각 프로세스의 waiting time
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time


    gantt_cnt = 0;


    for (int i = 0; i < process_num; i++) {

        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;

    }

    while (finished < process_num) {

        int idx = -1;

        for (int i = 0; i < process_num; i++) {

            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue;  // I/O 중이면 건너뜀
            if (ready_queue[i].arrival > time) continue;    // process가 time 시간 내에 도착해 있어야 처리 가능

            if (idx == -1 || (remaining_burst[i] < remaining_burst[idx]) || ((remaining_burst[i] == remaining_burst[idx]) && (ready_queue[i].arrival < ready_queue[idx].arrival)))   //cpu burst가 짧은 process 혹은 만일 cpu burst가 같다면 먼저 도착한 processf를 선택택
                idx = i;
        }

        // 모든 process가 time 내에 있지 않을 경우
        if (idx == -1) {
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }

        int exec_time = remaining_burst[idx];  // 현재 burst 전체를 실행할 수 있다고 가정

        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int io_request = ready_queue[idx].IO_request[io_index[idx]];
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (io_request - progress < exec_time) {
                exec_time = io_request - progress;
            }
        }

        if (exec_time > 0) {
            time += exec_time;
            remaining_burst[idx] -= exec_time;
            write_Gantt(idx, time, exec_time, 0, 0);

        }

        // 지금 실행 중인 process가 I/O 요청 시점에 도달했는지 확인
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (progress == ready_queue[idx].IO_request[io_index[idx]]) {

                process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // I/O 요청 발생 시 해당 process의 waiting time

                wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                ready_queue[idx].arrival = wait_until[idx];   // I/O 완료 후 arrival 시간 갱신
                io_index[idx]++;
                continue;  // I/O 처리 후 다시 ready 상태로 돌아감
            }
        }

        // process가 완전히 끝났을 때 해당 process의 waiting time과 turnaround time 계산산
        if (remaining_burst[idx] == 0) {

            process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // 각 process의 waiting time 계산
            total_wait[1] += process_wait[idx];     // 각 process의 waiting time을 더해 total waiting time을 구함
            process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
            total_turnaround[1] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산산

            finished++;

        }

    }

    print_process_wait_turnaroud("SJF", process_wait, process_turnaround);

    print_Gantt("SJF");

    return;

}

void Priority(void) {

    int time = 0;
    int finished = 0;

    total_wait[2] = 0;
    total_turnaround[2] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];   // 각 프로세스의 현재 IO 처리 인덱스
    int wait_until[MAX]; // I/O 끝나는 시점 저장
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };  // 각 프로세스의 waiting time
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time


    gantt_cnt = 0;


    for (int i = 0; i < process_num; i++) {

        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;

    }

    while (finished < process_num) {

        int idx = -1;

        for (int i = 0; i < process_num; i++) {

            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue;  // I/O 중이면 건너뜀
            if (ready_queue[i].arrival > time) continue;    // process가 time 시간 내에 도착해 있어야 처리 가능

            if (idx == -1 || (ready_queue[i].priority > ready_queue[idx].priority) || ((ready_queue[i].priority == ready_queue[idx].priority) && (ready_queue[i].arrival < ready_queue[idx].arrival)))   //priority가 높은 process(larger priority is higher priority) 혹은 만일 priority가 같다면 먼저 도착한 process를 선택
                idx = i;
        }

        // 모든 process가 time 내에 있지 않을 경우
        if (idx == -1) {
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }

        int exec_time = remaining_burst[idx];  // 현재 burst 전체를 실행할 수 있다고 가정

        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int io_request = ready_queue[idx].IO_request[io_index[idx]];
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (io_request - progress < exec_time) {
                exec_time = io_request - progress;
            }
        }

        if (exec_time > 0) {
            time += exec_time;
            remaining_burst[idx] -= exec_time;
            write_Gantt(idx, time, exec_time, 0, 0);

        }

        // 지금 실행 중인 process가 I/O 요청 시점에 도달했는지 확인
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (progress == ready_queue[idx].IO_request[io_index[idx]]) {

                process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // I/O 요청 발생 시 해당 process의 waiting time

                wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                ready_queue[idx].arrival = wait_until[idx];   // I/O 완료 후 arrival 시간 갱신
                io_index[idx]++;
                continue;  // I/O 처리 후 다시 ready 상태로 돌아감
            }
        }

        // process가 완전히 끝났을 때 해당 process의 waiting time과 turnaround time 계산산
        if (remaining_burst[idx] == 0) {

            process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // 각 process의 waiting time 계산
            total_wait[2] += process_wait[idx];     // 각 process의 waiting time을 더해 total waiting time을 구함
            process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
            total_turnaround[2] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산산

            finished++;
        }

    }

    print_process_wait_turnaroud("Priority", process_wait, process_turnaround);


    print_Gantt("Priority");

    return;

}

void RR(void) {

    int time_quantum;
    printf("\nEnter the time quantum of RR: ");
    scanf("%d", &time_quantum);

    int time = 0;
    int finished = 0;

    total_wait[3] = 0;
    total_turnaround[3] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];   // 각 프로세스의 현재 IO 처리 인덱스
    int wait_until[MAX]; // I/O 끝나는 시점 저장
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };  // 각 프로세스의 waiting time
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time


    gantt_cnt = 0;


    for (int i = 0; i < process_num; i++) {

        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;

    }

    while (finished < process_num) {


        int idx = -1;

        for (int i = 0; i < process_num; i++) {

            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue;  // I/O 중이면 건너뜀
            if (ready_queue[i].arrival > time) continue;    // process가 time 시간 내에 도착해 있어야 처리 가능

            if (idx == -1 || (ready_queue[i].arrival < ready_queue[idx].arrival))
                idx = i;
        }

        if (idx == -1) {
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }


        if (remaining_burst[idx] <= time_quantum) {

            int exec_time = remaining_burst[idx];  // 현재 burst 전체를 실행할 수 있다고 가정

            if (io_index[idx] < ready_queue[idx].IO_cnt) {
                int io_request = ready_queue[idx].IO_request[io_index[idx]];
                int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
                if (io_request - progress < exec_time) {
                    exec_time = io_request - progress;
                }
            }

            if (exec_time > 0) {
                time += exec_time;
                remaining_burst[idx] -= exec_time;
                write_Gantt(idx, time, exec_time, 0, 0);

            }

            // 지금 실행 중인 process가 I/O 요청 시점에 도달했는지 확인
            if (io_index[idx] < ready_queue[idx].IO_cnt) {
                int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
                if (progress == ready_queue[idx].IO_request[io_index[idx]]) {

                    process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // I/O 요청 발생 시 해당 process의 waiting time

                    wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                    ready_queue[idx].arrival = wait_until[idx];   // I/O 완료 후 arrival 시간 갱신
                    io_index[idx]++;
                    continue;  // I/O 처리 후 다시 ready 상태로 돌아감
                }
            }


            if (remaining_burst[idx] == 0) {

                process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // 각 process의 waiting time 계산
                total_wait[3] += process_wait[idx];     // 각 process의 waiting time을 더해 total waiting time을 구함
                process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
                total_turnaround[3] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산산

                finished++;
            }
        }
        else {

            int exec_time = time_quantum;
            int io_handled = 0; // I/O 처리 여부

            if (io_index[idx] < ready_queue[idx].IO_cnt) {

                int io_request = ready_queue[idx].IO_request[io_index[idx]];
                int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
                int distance_to_io = io_request - progress;

                if ((distance_to_io > 0) && (distance_to_io <= exec_time)) {
                    exec_time = distance_to_io; // I/O 요청 시점까지 실행
                    io_handled = 1; // I/O를 처리함
                }
            }

            if (exec_time > 0) {
                time += exec_time;
                remaining_burst[idx] -= exec_time;
                write_Gantt(idx, time, exec_time, 0, 0);

            }

            // I/O 요청 발생 시점이 time quantum 내에 있을 때, I/O 처리
            if (io_handled) {
                int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
                if (progress == ready_queue[idx].IO_request[io_index[idx]]) {

                    process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);   // I/O 요청 발생 시 해당 process의 waiting time

                    wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                    ready_queue[idx].arrival = wait_until[idx];   // I/O 완료 후 arrival 시간 갱신
                    io_index[idx]++;
                    continue;  // I/O 처리 후 다시 ready 상태로 돌아감
                }
            }

            // time quantum 내에 I/O가 발생하지 않았을 때
            process_wait[idx] += ((time - exec_time) - ready_queue[idx].arrival);

            ready_queue[idx].arrival = time;


        }



    }

    print_process_wait_turnaroud("Round Robin", process_wait, process_turnaround);

    print_Gantt("Round Robin");

    return;

}

void Preemtive_SJF(void) {

    int time = 0;
    int finished = 0;

    total_wait[4] = 0;
    total_turnaround[4] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];          // 현재 IO 처리 인덱스
    int wait_until[MAX];        // I/O 끝나는 시점
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time


    int last_exe[MAX] = { -1 };     // 마지막으로 실행이 시작된 시점
    int first_exe[MAX]; // 처음 실행인지 체크

    gantt_cnt = 0;
    int current_process = -1;
    int start_current = 0;


    for (int i = 0; i < process_num; i++) {
        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;
        first_exe[i] = 1;
    }

    while (finished < process_num) {
        int idx = -1;

        for (int i = 0; i < process_num; i++) {
            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue; // I/O 중이면 skip
            if (ready_queue[i].arrival > time) continue; // 아직 도착 안함

            if (idx == -1 || remaining_burst[i] < remaining_burst[idx] || (remaining_burst[i] == remaining_burst[idx] && ready_queue[i].arrival < ready_queue[idx].arrival)) {
                idx = i;
            }
        }

        if (idx == -1) {

            if (current_process != -1) {
                write_Gantt(current_process, time, 0, 1, start_current);
                current_process = -1;
            }
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }

        // 실행 중인 process가 바뀌었을 때 간트 차트를 기록함
        if (current_process != idx) {

            if (current_process != -1)
                write_Gantt(current_process, time, 0, 1, start_current);

            current_process = idx;
            start_current = time;

        }



        // 처음 실행된 경우 대기 시간 계산을 위해 저장
        if (first_exe[idx]) {
            process_wait[idx] += time - ready_queue[idx].arrival;
            first_exe[idx] = 0;
        }
        else {
            if (ready_queue[idx].arrival > last_exe[idx]) {
                // I/O request 후에 arrival time이 갱신되었을 경우
                process_wait[idx] += time - ready_queue[idx].arrival;
            }
            else if (last_exe[idx] != time - 1) {
            // 다시 실행되는 경우에도 대기 시간 추가
                process_wait[idx] += time - (last_exe[idx] + 1);
            }
        }

        remaining_burst[idx]--;
        time++;
        last_exe[idx] = time - 1;

        // I/O 요청 체크
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (progress == ready_queue[idx].IO_request[io_index[idx]]) {
                wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                ready_queue[idx].arrival = wait_until[idx];  // 다시 도착하는 시점 갱신
                io_index[idx]++;
                continue;
            }
        }

        // 프로세스 종료
        if (remaining_burst[idx] == 0) {

            total_wait[4] += process_wait[idx];
            process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
            total_turnaround[4] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산산

            finished++;
        }
    }

    if (current_process != -1)
        write_Gantt(current_process, time, 0, 1, start_current);

    print_process_wait_turnaroud("Preemtive_SJF", process_wait, process_turnaround);


    print_Gantt("Preemtive_SJF");

    return;

}



void Preemtive_Priority(void) {


    int time = 0;
    int finished = 0;

    total_wait[5] = 0;
    total_turnaround[5] = 0;

    int remaining_burst[MAX];
    int io_index[MAX];   // I/O 인덱스
    int wait_until[MAX]; // I/O 끝나는 시점
    int ori_arrival[MAX];
    int process_wait[MAX] = { 0 };
    int process_turnaround[MAX] = { 0 }; // 각 프로세스의 turnaround time


    int last_exe[MAX] = { -1 };     // 마지막으로 실행된 시점
    int first_exe[MAX]; // 처음 실행인지 체크

    gantt_cnt = 0;
    int current_process = -1;
    int start_current = 0;


    for (int i = 0; i < process_num; i++) {

        remaining_burst[i] = ready_queue[i].cpu_burst;
        io_index[i] = 0;
        wait_until[i] = -1;
        ori_arrival[i] = ready_queue[i].arrival;
        first_exe[i] = 1;

    }

    while (finished < process_num) {

        int idx = -1;

        for (int i = 0; i < process_num; i++) {
            if (remaining_burst[i] == 0) continue;
            if (wait_until[i] > time) continue;
            if (ready_queue[i].arrival > time) continue;

            if (idx == -1 || ready_queue[i].priority > ready_queue[idx].priority || (ready_queue[i].priority == ready_queue[idx].priority && ready_queue[i].arrival < ready_queue[idx].arrival)) {
                idx = i;
            }
        }

        if (idx == -1) {
            if (current_process != -1) {
                write_Gantt(current_process, time, 0, 1, start_current);
                current_process = -1;
            }
            time++;
            write_Gantt(idx, time, 1, 0, 0);    // IDLE 상태 Gantt chart에 기록
            continue;
        }

        // 실행 중인 process가 바뀌었을 때 간트 차트를 기록함
        if (current_process != idx) {

            if(current_process != -1)
                write_Gantt(current_process, time, 0, 1, start_current);
            current_process = idx;
            start_current = time;

        }


        // 처음 실행되거나 다시 실행될 때 대기시간 누적
        if (first_exe[idx]) {
            process_wait[idx] += time - ready_queue[idx].arrival;
            first_exe[idx] = 0;
        }
        else {
            if (ready_queue[idx].arrival > last_exe[idx]) {
                // I/O request 후에 arrival time이 갱신되었을 경우
                process_wait[idx] += time - ready_queue[idx].arrival;
            }
            else if (last_exe[idx] != time - 1) {
                // 다시 실행되는 경우에도 대기 시간 추가
                process_wait[idx] += time - (last_exe[idx] + 1);
            }
        }

        remaining_burst[idx]--;
        time++;
        last_exe[idx] = time - 1;

        // I/O 요청 발생 체크
        if (io_index[idx] < ready_queue[idx].IO_cnt) {
            int progress = ready_queue[idx].cpu_burst - remaining_burst[idx];
            if (progress == ready_queue[idx].IO_request[io_index[idx]]) {
                wait_until[idx] = time + ready_queue[idx].IO_burst[io_index[idx]];
                ready_queue[idx].arrival = wait_until[idx];
                io_index[idx]++;
                continue;
            }
        }

        // 프로세스 종료
        if (remaining_burst[idx] == 0) {

            total_wait[5] += process_wait[idx];
            process_turnaround[idx] += (time - ori_arrival[idx]);   // 각 process의 turnaround time 계산
            total_turnaround[5] += process_turnaround[idx];   // 각 process의 turnaroud time을 더해 total turnaroud time 계산산

            finished++;
        }
    }

    if(current_process != -1)
        write_Gantt(current_process, time, 0, 1, start_current);


    print_process_wait_turnaroud("Preemtive_Priority", process_wait, process_turnaround);

    print_Gantt("Preemtive_Priority");

    return;

}



void Evaluation(void) {

    float avr_wait[6]; // 각 스케쥴링 알고리즘들 간의 avr waiting time
    float avr_turnaround[6]; // 각 스케쥴링 알고리즘들 간의 avr turnaround time

    char* alg[6] = { "FCFS","SJF", "Priority","RR", "Preemtive_SJF", "Preemtive_Priority" };

    printf("\n\n-------------------------------------- Evaluation Algorithm\n");
    for (int i = 0; i < 6; i++) {

        avr_wait[i] = (float)total_wait[i] / process_num;
        avr_turnaround[i] = (float)total_turnaround[i] / process_num;

        printf("\n%s:\n", alg[i]);
        printf("Average waiting time: %.2f\n", avr_wait[i]);
        printf("Average turnaround time: %.2f\n", avr_turnaround[i]);
    }


    int wait_min = 0;
    int turnaround_min = 0;

    float wmin = avr_wait[0];
    float tmin = avr_turnaround[0];

    for (int i = 0; i < 6; i++) {

        if (avr_wait[i] < wmin) {
            wmin = avr_wait[i];
            wait_min = i;
        }

        if (avr_turnaround[i] < tmin) {
            tmin = avr_turnaround[i];
            turnaround_min = i;
        }
    }

    printf("\nAlgorithm with minumum average waiting time is %s\n", alg[wait_min]);
    printf("\nAlgorithm with minumum average turnaround time is %s\n", alg[turnaround_min]);

    return;

}