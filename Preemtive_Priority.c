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
            process_wait[idx] += time - ori_arrival[idx];
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
