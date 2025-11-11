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