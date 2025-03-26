# Question 3: Multi-Process Merge Sort Implementation

## Process Overview

The implementation follows these key steps:

### 1. Determining Partitions
- Number of child processes (M) is predefined
- Array size N is divided into M equal partitions
- Each partition size is calculated as `chunk_size = N / M`

### 2. Process Creation and Sorting
- The array is divided into M partitions of equal size
- For each partition:
  - A child process is created using `fork()`
  - Child process performs merge sort on its assigned partition
  - Child process exits after sorting is complete

### 3. Parent Process Coordination
- Parent process waits for all child processes using `waitpid()`
- Once all children complete, parallel merging phase begins

### 4. Merging Strategy
The merging phase is also parallelized:
- For each merging level:
  - Calculate number of merges needed: `num_merges = (n + 2 * size - 1) / (2 * size)`
  - Create a new process for each merge operation
  - Each merge process handles combining two adjacent sorted sections:
    - First section: `[left ... mid]`
    - Second section: `[mid+1 ... right]`
  - Parent waits for all merge processes at current level to complete
  - Size doubles for next level: `size *= 2`

Example of parallel merging with N=16, M=4:
```
Initial sorted chunks:
[1,4,6,7] [2,5,8,9] [0,3,8,9] [1,2,6,7]

Level 1 (size=4):
Process 1: merges [1,4,6,7] and [2,5,8,9] → [1,2,4,5,6,7,8,9]
Process 2: merges [0,3,8,9] and [1,2,6,7] → [0,1,2,3,6,7,8,9]
(These run in parallel)

Level 2 (size=8):
Process 1: merges the two sorted halves → Final sorted array
```

## Performance Analysis for N = 2^20

Testing was conducted on an Ubuntu server VM with the following specifications:
- 4 CPU cores
- 4GB RAM
- Array size: 1,048,576 elements (2^20)

***-I also included the test_performance.c file so you can test the performance in your own computer***

### Results:
One of the test_performance run:

| Number of Processes (M) | Time (seconds) |
|------------------------|----------------|
| 1                      | 0.116          |
| 2                      | 0.053          |
| 4                      | 0.034          |
| 8                      | 0.033          |
| 16                     | 0.035          |
| 32                     | 0.037          |
| 64                     | 0.041          |
| 128                    | 0.048          |
| 256                    | 0.059          |
| 512                    | 0.090          |
| 1024                   | 0.136          |
| 2048                   | 0.266          |
| 4096                   | 0.568          |
| 8192                   | 0.986          |
| 16384                  | 1.704          |
| 32768                  | 3.121          |


<br></br>
<br></br>
<br></br>
Another run:

| Number of Processes (M) | Time (seconds) |
|------------------------|-----------------|
| 1                      | 0.115           |
| 2                      | 0.053           |
| 4                      | 0.033           |
| 8                      | 0.033           |
| 16                     | 0.035           |
| 32                     | 0.036           |
| 64                     | 0.041           |
| 128                    | 0.045           |
| 256                    | 0.057           |
| 512                    | 0.086           |
| 1024                   | 0.142           |
| 2048                   | 0.255           |
| 4096                   | 0.486           |
| 8192                   | 0.922           |
| 16384                  | 1.653           |
| 32768                  | 3.002           |


# Comparing M=1 with the best M

## Key Findings

1. **Sequential Performance (M=1)**:
   - Run 1: 116.0 ms
   - Run 2: 115.0 ms
   - Average: 115.5 ms

2. **Optimal Parallel Performance**:
   - M=4: ~33.5 ms (average)
   - M=8: ~33.0 ms (average)

3. **Speedup Achieved**:
   - Using M=4: ~71% improvement
   - Using M=8: ~71.5% improvement

## Analysis

1. **Parallelization Benefits**:
   - The parallel implementation (both M=4 and M=8) achieves approximately 3.4x speedup compared to the sequential version
   - This indicates efficient utilization of available CPU cores

2. **Consistency**:
   - Both runs show very consistent performance
   - The variance between runs is minimal (within 1 ms)
   - This suggests stable and reliable performance improvements

3. **Diminishing Returns**:
   - The minimal difference between M=4 and M=8 indicates reaching the hardware's parallel processing capacity
   - Additional processes beyond M=8 lead to decreased performance due to overhead


# Conclusion

The optimal number of processes (M) for N = 2^20 varied between 4 and 8 processes in testing, with 8 processes showing better performance more frequently (in approximately 6 out of 10 test runs). This variation is expected in real-world conditions due to:
1. System load fluctuations during testing
2. Process scheduling variations
3. Memory access patterns and cache behavior
4. Background system activities
Over multiple test runs (10 trials), the results showed:
- M = 8 was optimal in ~60% of runs
- M = 4 was optimal in ~40% of runs
- Performance differences between M = 4 and M = 8 were often small (within one or two milliseconds)

Performance degraded significantly as M increased beyond 8 due to:
1. Process management overhead for both initial sorting and merging phases
2. Increased number of merge processes required at each level
3. Synchronization overhead between merge levels
4. Resource contention with limited CPU cores (4 in this case)

The parallel merging strategy helps utilize available CPU cores during the merge phase, but the benefits are limited by the hardware constraints and the overhead of process creation and management. The variation between 4 and 8 processes being optimal likely reflects the balance point between parallelization benefits and process management overhead on this specific hardware configuration.