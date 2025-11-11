# OS_termproject

## CPU 스케줄링 시뮬레이터

### 프로젝트 개요

이 프로젝트는 **운영체제(Operating Systems)** 수업의 텀프로젝트로 수행한 **CPU 스케줄링 시뮬레이터**입니다.
운영체제의 핵심 기능 중 하나인 **프로세스 스케줄링**을 실제로 구현하여, 다양한 알고리즘별로 CPU 자원 분배 방식을 비교하고 **평균 대기시간, 반환시간, CPU 활용률** 등을 측정합니다.
C 언어로 작성되었으며, 각 알고리즘의 동작 원리를 시각적으로 이해할 수 있도록 **간트 차트(Gantt Chart)** 출력 기능도 포함되어 있습니다.

---

### 구현된 스케줄링 알고리즘

* FCFS (First-Come, First-Served) – Non-preemptive, simple queue-based scheduling
* SJF (Shortest Job First) – Both preemptive and non-preemptive versions implemented
* Priority Scheduling – Supports preemptive and non-preemptive modes
* Round Robin (RR) – Implements time-slicing with a configurable quantum
* (Optional) I/O handling and Gantt chart visualization included

---

### 주요 기능

* 프로세스 정보 입력 (도착시간, 실행시간, 우선순위 등)
* 다양한 스케줄링 알고리즘 선택 및 결과 비교
* 평균 대기시간, 평균 반환시간, CPU 활용률 계산
* 실행 순서를 보여주는 **간트 차트 시각화**
* I/O 요청을 포함한 **실제 OS 환경 유사 시뮬레이션**

---

### 예시 출력

```
[Algorithm] Preemptive Priority Scheduling
-------------------------------------------------
PID | Arrival | Burst | Priority | Waiting | Turnaround
-------------------------------------------------
P1  |    0    |   6   |     2    |    4    |     10
P2  |    2    |   4   |     1    |    2    |      6
P3  |    3    |   5   |     3    |    8    |     13
-------------------------------------------------
Average Waiting Time   : 4.67
Average Turnaround Time: 9.67
CPU Utilization        : 96.5%
```

---

### 개발 환경

* 언어: C
* 핵심 주제: Process scheduling, preemption, I/O handling, CPU utilization

---

### 🎯 학습 목표

* 운영체제의 프로세스 스케줄링 개념과 각 알고리즘의 동작 차이를 이해
* 선점형 / 비선점형 스케줄링의 동작 과정을 직접 구현 및 실험
* 시뮬레이션을 통해 CPU 성능 지표(대기시간, 반환시간, 응답시간 등)를 비교 분석
