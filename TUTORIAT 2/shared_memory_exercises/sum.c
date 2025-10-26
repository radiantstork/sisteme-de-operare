#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>

// Computes the sum of the first 10 numbers using two processes
// Parent sums the first half, child sums the second half
// The child will store it's sum in the last element

int main() {
    const char *shm_name = "/shared_sum";
    const int total_elems = 10;   
    const int shm_size = total_elems * sizeof(int); 

    // Create shared memory
    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Set size of shared memory
    if (ftruncate(fd, shm_size) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Map shared memory
    int *shared = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd); // descriptor no longer needed after mmap

    // Compute half and store it
    int half = total_elems / 2;

    // Fill shared memory with numbers (starting after index 1)
    printf("Parent: Writing %d numbers to shared memory:\n", total_elems);
    for (int i = 0; i < total_elems; i++) {
        shared[i] = i + 1;
        printf("%d ", shared[i]);
    }
    printf("\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // Child executes
        int start = half;
        int sum = 0;

        printf("Child: Summing elements from index %d to %d\n", start, start + half - !(total_elems % 2));
        for (int i = start; i < start + half + total_elems % 2; i++) {
            sum += shared[i];
        }

        shared[total_elems - 1] = sum; // last element holds child's sum
        printf("Child: Partial sum = %d\n", sum);

        munmap(shared, shm_size);
        exit(0);
    } 
    else { // Parent executes
        int sum_parent = 0;
        int start = 0;

        printf("Parent: Summing elements from index %d to %d\n", start, start + half - 1);
        for (int i = start; i < start + half; i++) {
            sum_parent += shared[i];
        }

        // Wait for child to finish
        wait(NULL);

        int sum_child = shared[total_elems - 1];
        int total_sum = sum_parent + sum_child;

        printf("Parent: Partial sum = %d\n", sum_parent);
        printf("Parent: Child's sum = %d\n", sum_child);
        printf("Parent: Total sum = %d\n", total_sum);

        // Cleanup
        munmap(shared, shm_size);
        shm_unlink(shm_name);
    }

    return 0;
}