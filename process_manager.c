#include "process_manager.h"

/*
 * Function 1: Basic Producer-Consumer Demo
 * Creates one producer child (sends 1,2,3,4,5) and one consumer child (adds them up)
 */

int run_basic_demo(void) {
    int pipe_fd[2];
    pid_t producer_pid, consumer_pid;
    int status;
    
    printf("\nParent process (PID: %d) creating children...\n", getpid());
    
    if (pipe(pipe_fd) == -1) {

        perror("pipe");
        return -1;

    }

    producer_pid = fork();

    if (producer_pid == 0) {

        close(pipe_fd[0]);
        producer_process(pipe_fd[1], 1);
        exit(0);

    } else if (producer_pid < 0) {

        perror("fork (producer)");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;

    } else {

        printf("Created Producer Child (PID: %d)\n", producer_pid);

    }

    consumer_pid = fork();

    if (consumer_pid == 0) {

        close(pipe_fd[1]);
        consumer_process(pipe_fd[0], 0);
        exit(0);

    } else if (consumer_pid < 0) {

        perror("fork (consumer)");
        close (pipe_fd[0]);
        close (pipe_fd[1]);

        waitpid(producer_pid, &status, 0);

        return -1;

    } else {

        printf("Created Consumer Child (PID: %d)\n", consumer_pid);

    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    if (waitpid(producer_pid, &status, 0) == -1) {

        perror("waitpid (producer)");
        return -1;

    }

    if (WIFEXITED(status)) {

        printf("Producer Child (PID: %d) exited with status %d\n", producer_pid, WEXITSTATUS(status));

    } else {

        printf("Producer Child (PID: %d) terminated abnormally \n", producer_pid);

    }

    if (waitpid(consumer_pid, &status, 0) == -1) {

        perror("waitpid (consumer)");
        return -1;

    }

    if (WIFEXITED(status)) {

        printf("Consumer Child (PID: %d) exited with status %d\n", consumer_pid, WEXITSTATUS(status));

    } else {

        printf("Consumer Child (PID: %d) terminated abnormally\n", consumer_pid);

    }


    return 0;
}

//Function 2: Multiple Producer-Consumer Pairs
int run_multiple_pairs(int num_pairs) {

    // stores all child PIDs
    pid_t pids[10]; 
    int pid_count = 0;

    printf("\nParent creating %d producer-consumer pairs...\n", num_pairs);

    for (int i = 0; i < num_pairs; i++) {

        int pipe_fd[2];
        pid_t producer_pid, consumer_pid;

        printf("\n=== Pair %d ===\n", i + 1);

        if (pipe(pipe_fd) == -1) {

            perror("pipe");
            return -1;

        }

        producer_pid = fork();

        if (producer_pid == 0) {

            close(pipe_fd[0]);
            producer_process(pipe_fd[1], i * 5 + 1);
            exit(0);

        } else if (producer_pid < 0) {

            perror("fork (producer)");
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            return -1;

        }

        consumer_pid = fork();

        if (producer_pid == 0) {

            close(pipe_fd[1]);
            consumer_process(pipe_fd[0], i + 1);
            exit(0);

        } else if (consumer_pid < 0) {

            perror("fork (consumer)");
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            waitpid(producer_pid, NULL, 0);

            return -1;

        }

        pids[pid_count++] = producer_pid;
        pids[pid_count++] = consumer_pid;

        close(pipe_fd[0]);
        close(pipe_fd[1]);

    }

	if (pid_count == 0) {

		printf("No Child Processes To Wait For\n");
		return 0;

	}

    for (int i = 0; i < pid_count; i++) {

        int status;
        pid_t w = waitpid(pids[i], &status, 0);

        if (w == -1) {

            perror("waitpid");
            continue;

        }

        if (WIFEXITED(status)) {

            printf("Child (PID: %d) Exited With Status %d\n", w, WEXITSTATUS(status));

        } else {

            printf("Child (PID: %d) Terminated Abnormally\n", w);

        }

    }

    printf("\nAll pairs completed successfully!\n");

    return 0;
}


void producer_process(int write_fd, int start_num) {
    printf("Producer (PID: %d) starting...\n", getpid());
    
    // Send 5 numbers: start_num, start_num+1, start_num+2, start_num+3, start_num+4
    for (int i = 0; i < NUM_VALUES; i++) {

        int number = start_num + i;
        
        if (write(write_fd, &number, sizeof(number)) != sizeof(number)) {

            perror("write");
            exit(1);

        }
        
        printf("Producer: Sent number %d\n", number);
        // Small delay to see output clearly
        usleep(100000); 
    }
    
    printf("Producer: Finished sending %d numbers\n", NUM_VALUES);
    close(write_fd);
    exit(0);
}

/*
 * Consumer Process - Receives numbers and calculates sum
 */
void consumer_process(int read_fd, int pair_id) {
    int number;
    int count = 0;
    int sum = 0;
    
    printf("Consumer (PID: %d) starting...\n", getpid());
    
    // Read numbers until pipe is closed
    while (read(read_fd, &number, sizeof(number)) > 0) {

        count++;
        sum += number;
        printf("Consumer: Received %d, running sum: %d\n", number, sum);

    }
    
    printf("Consumer: Final sum: %d\n", sum);
    close(read_fd);
    exit(0);

}
